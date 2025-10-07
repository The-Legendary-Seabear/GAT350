#pragma once
#include "Event.h"
#include "Observer.h"
#include "Core/Singleton.h"
#include <map>
#include <list>

/// <summary>
/// Convenience macro for registering an observer to receive notifications for a specific event type.
/// 
/// This macro simplifies the registration process by automatically converting the event ID to a string
/// and passing the current object as the observer. It's designed to be called from within classes
/// that implement IObserver.
/// 
/// The macro expands to: neu::EventManager::Instance().AddObserver("event_id", *this)
/// 
/// Usage: Place this macro in the constructor or initialization method of observer classes.
/// 
/// Example:
/// ```cpp
/// class HealthBar : public IObserver {
/// public:
///     HealthBar() {
///         ADD_OBSERVER(player_health_changed);  // Register for health events
///         ADD_OBSERVER(player_died);            // Register for death events
///     }
/// };
/// ```
/// 
/// Benefits:
/// - Reduces boilerplate code for observer registration
/// - Automatic string conversion prevents typos in event IDs
/// - Consistent registration pattern across the codebase
/// - Self-documenting: clearly shows which events a class observes
/// </summary>
/// <param name="event_id">The event identifier (without quotes) to register for</param>
#define ADD_OBSERVER(event_id)				neu::EventManager::Instance().AddObserver(#event_id, *this)

/// <summary>
/// Convenience macro for unregistering an observer from all event notifications.
/// 
/// This macro removes the current object from all event subscriptions in the EventManager.
/// It's typically called automatically from the IObserver destructor but can also be used
/// for manual unregistration when an observer should stop receiving events before destruction.
/// 
/// The macro expands to: neu::EventManager::Instance().RemoveObserver(*this)
/// 
/// Usage examples:
/// ```cpp
/// class GameState : public IObserver {
/// public:
///     void EnterPauseMode() {
///         REMOVE_OBSERVER;  // Stop receiving events while paused
///     }
///     
///     void ExitPauseMode() {
///         ADD_OBSERVER(player_action);  // Re-register for relevant events
///     }
/// };
/// ```
/// 
/// Automatic Usage:
/// This macro is automatically called by the IObserver destructor, ensuring proper cleanup
/// without requiring manual intervention in most cases.
/// </summary>
#define REMOVE_OBSERVER						neu::EventManager::Instance().RemoveObserver(*this)

/// <summary>
/// Convenience macro for sending events with associated data through the event system.
/// 
/// This macro creates an Event object with the specified ID and data, then immediately
/// dispatches it to all registered observers. The event ID is automatically converted
/// to a string, and the data can be any type supported by the Event::data_t variant.
/// 
/// The macro expands to: neu::EventManager::Instance().Notify({ "event_id", data })
/// 
/// Usage examples:
/// ```cpp
/// // Send events with different data types
/// EVENT_NOTIFY_DATA(player_health_changed, 75);           // int data
/// EVENT_NOTIFY_DATA(player_speed_boost, 1.5f);            // float data
/// EVENT_NOTIFY_DATA(level_name_changed, std::string("Forest")); // string data
/// EVENT_NOTIFY_DATA(player_position_updated, vec2{100, 200}); // vec2 data
/// ```
/// 
/// Benefits:
/// - Concise syntax for event dispatch with data
/// - Automatic string conversion for event IDs
/// - Type-safe data handling through variant system
/// - Immediate event delivery to all registered observers
/// </summary>
/// <param name="event_id">The event identifier (without quotes) to send</param>
/// <param name="data">The data payload to include with the event</param>
#define EVENT_NOTIFY_DATA(event_id, data)	neu::EventManager::Instance().Notify({ #event_id, data })

/// <summary>
/// Convenience macro for sending simple notification events without specific data.
/// 
/// This macro creates an Event with the specified ID and a default boolean value of true.
/// It's useful for events that represent state changes or triggers that don't require
/// additional data beyond the fact that they occurred.
/// 
/// The macro expands to: neu::EventManager::Instance().Notify({ "event_id", true })
/// 
/// Usage examples:
/// ```cpp
/// EVENT_NOTIFY(level_complete);      // Simple completion notification
/// EVENT_NOTIFY(game_paused);         // State change notification
/// EVENT_NOTIFY(player_died);         // Event occurrence notification
/// EVENT_NOTIFY(audio_music_stop);    // Command-style notification
/// ```
/// 
/// When to use:
/// - State change notifications where the change itself is the important information
/// - Trigger events that don't need associated data
/// - Command-style events that instruct systems to perform actions
/// - Simple boolean flags or status updates
/// </summary>
/// <param name="event_id">The event identifier (without quotes) to send</param>
#define EVENT_NOTIFY(event_id)				neu::EventManager::Instance().Notify({ #event_id, true })

namespace neu {
    /// <summary>
    /// Central event management system implementing the Observer pattern for game-wide event dispatch.
    /// 
    /// The EventManager serves as the central hub for all event-driven communication in the engine.
    /// It maintains subscriber lists for different event types and handles the dispatch of events
    /// to all interested observers. This enables loose coupling between game systems while
    /// providing efficient, type-safe event communication.
    /// 
    /// Key Features:
    /// - Observer pattern implementation for decoupled communication
    /// - Event type-based subscription system for efficient filtering
    /// - Singleton design for global accessibility
    /// - Thread-safe observer management (implementation dependent)
    /// - Support for multiple observers per event type
    /// - Automatic cleanup when observers are destroyed
    /// 
    /// Architecture Benefits:
    /// - Decoupled systems: Event producers don't know about consumers
    /// - Dynamic relationships: Observers can subscribe/unsubscribe at runtime
    /// - Centralized dispatch: Single point of control for all event communication
    /// - Type safety: Leverages Event struct's variant-based type system
    /// - Performance: Direct notification without message queuing overhead
    /// 
    /// Common Usage Patterns:
    /// 1. **System Communication**: Audio system responding to gameplay events
    /// 2. **UI Updates**: Interface elements reacting to game state changes
    /// 3. **Achievement Tracking**: Monitoring player actions for unlock conditions
    /// 4. **State Synchronization**: Keeping multiple systems in sync
    /// 5. **Debug/Logging**: Tracking events for development and debugging
    /// 
    /// Event Flow:
    /// 1. Observers register for specific event types using AddObserver()
    /// 2. Event producers create and send events using Notify()
    /// 3. EventManager dispatches events to all registered observers
    /// 4. Each observer's OnNotify() method is called with the event
    /// 5. Observers process events according to their specific logic
    /// 
    /// Performance Considerations:
    /// - Event dispatch is synchronous and immediate
    /// - Large numbers of observers may impact performance
    /// - Consider event filtering and early exit strategies
    /// - Observer order is not guaranteed (based on list iteration)
    /// 
    /// Thread Safety:
    /// The current implementation may not be thread-safe. Consider adding
    /// synchronization if events are sent from multiple threads or if
    /// observers may be added/removed concurrently.
    /// </summary>
    class EventManager : public Singleton<EventManager> {
    public:
        /// <summary>
        /// Registers an observer to receive notifications for a specific event type.
        /// 
        /// This method adds the observer to the subscription list for the given event ID.
        /// When events with this ID are dispatched, the observer's OnNotify method will
        /// be called. Observers can register for multiple event types by calling this
        /// method multiple times with different IDs.
        /// 
        /// Subscription Management:
        /// - Observers are stored by reference, so they must remain valid
        /// - The same observer can register for multiple event types
        /// - Duplicate registrations for the same event type are allowed (may result in multiple notifications)
        /// - Registration is immediate and affects the next event dispatch
        /// 
        /// Memory Management:
        /// - Observers are stored as pointers for efficiency
        /// - The EventManager does not take ownership of observer objects
        /// - Observers must unregister themselves before destruction (handled automatically by IObserver destructor)
        /// 
        /// Usage examples:
        /// ```cpp
        /// // Manual registration (typically done via ADD_OBSERVER macro)
        /// EventManager::Instance().AddObserver("player_health_changed", healthBar);
        /// EventManager::Instance().AddObserver("player_health_changed", achievementTracker);
        /// 
        /// // Using convenience macro (preferred)
        /// ADD_OBSERVER(player_health_changed);
        /// ```
        /// 
        /// Error Handling:
        /// - No validation is performed on event IDs (any string is accepted)
        /// - Observer references must remain valid until unregistration
        /// - Consider using RAII patterns to ensure proper cleanup
        /// </summary>
        /// <param name="id">String identifier for the event type to observe</param>
        /// <param name="observer">Reference to the observer object that will receive notifications</param>
        void AddObserver(const Event::id_t& id, IObserver& observer);

        /// <summary>
        /// Removes an observer from all event subscriptions.
        /// 
        /// This method searches through all event subscription lists and removes the
        /// specified observer from each one. This is a complete unregistration that
        /// ensures the observer will no longer receive any event notifications.
        /// 
        /// Removal Process:
        /// - Iterates through all event type subscription lists
        /// - Removes the observer from any lists where it's found
        /// - Safe to call even if the observer wasn't registered
        /// - Safe to call multiple times with the same observer
        /// 
        /// Performance Considerations:
        /// - O(n*m) complexity where n = number of event types, m = average observers per type
        /// - Called automatically by IObserver destructor
        /// - Consider manual removal for observers with long lifetimes that temporarily don't need events
        /// 
        /// Usage examples:
        /// ```cpp
        /// // Manual removal (typically done via REMOVE_OBSERVER macro)
        /// EventManager::Instance().RemoveObserver(myObserver);
        /// 
        /// // Using convenience macro (preferred)
        /// REMOVE_OBSERVER;
        /// 
        /// // Automatic removal in IObserver destructor
        /// ~MyObserver() {
        ///     // REMOVE_OBSERVER is called automatically
        /// }
        /// ```
        /// 
        /// Thread Safety:
        /// If events can be dispatched while observers are being removed,
        /// additional synchronization may be required to prevent iterator
        /// invalidation or access to destroyed observer objects.
        /// </summary>
        /// <param name="observer">Reference to the observer to remove from all subscriptions</param>
        void RemoveObserver(IObserver& observer);

        /// <summary>
        /// Dispatches an event to all observers registered for its event type.
        /// 
        /// This method implements the core event dispatch logic, delivering the event
        /// to all observers that have registered for the event's ID. The notification
        /// is synchronous and immediate - all observers are notified before this method returns.
        /// 
        /// Dispatch Process:
        /// 1. Look up the event ID in the observer registry
        /// 2. If observers are found, iterate through the list
        /// 3. Call OnNotify() on each observer with the event
        /// 4. Continue even if individual observers throw exceptions (implementation dependent)
        /// 
        /// Event Delivery Guarantees:
        /// - Events are delivered to all registered observers
        /// - Delivery order is based on registration order (not guaranteed to be stable)
        /// - Events are delivered synchronously on the calling thread
        /// - No queuing or deferred delivery mechanisms
        /// 
        /// Performance Characteristics:
        /// - O(n) where n = number of observers for the event type
        /// - Direct function calls with minimal overhead
        /// - No dynamic memory allocation during dispatch
        /// - Events with no observers have minimal overhead (map lookup only)
        /// 
        /// Usage examples:
        /// ```cpp
        /// // Manual event creation and dispatch
        /// Event healthEvent("player_health_changed", 75);
        /// EventManager::Instance().Notify(healthEvent);
        /// 
        /// // Using convenience macros (preferred)
        /// EVENT_NOTIFY_DATA(player_health_changed, 75);
        /// EVENT_NOTIFY(level_complete);
        /// ```
        /// 
        /// Error Handling:
        /// - Events with unrecognized IDs are silently ignored (no observers)
        /// - Observer exceptions may propagate (implementation dependent)
        /// - Invalid event data is the responsibility of observers to handle
        /// 
        /// Thread Safety Considerations:
        /// - Event dispatch should typically happen on a single thread
        /// - Observer modifications during dispatch may cause undefined behavior
        /// - Consider event queuing for cross-thread communication
        /// </summary>
        /// <param name="event">The event to dispatch to registered observers</param>
        void Notify(const Event& event);

        /// <summary>
        /// Removes all observers from the event system.
        /// 
        /// This method clears all event subscriptions, effectively resetting the
        /// EventManager to its initial state. This is primarily used during shutdown
        /// or when completely reinitializing the event system.
        /// 
        /// Use cases:
        /// - Engine shutdown cleanup
        /// - Level transitions where all observers should be cleared
        /// - Testing scenarios requiring clean state
        /// - Error recovery situations
        /// 
        /// Warning: This method does not notify observers of their removal.
        /// Observers may still attempt to send events or expect to receive them
        /// after this call. Use with caution and ensure proper coordination
        /// with other systems.
        /// 
        /// After calling this method:
        /// - All event subscriptions are cleared
        /// - New observer registrations work normally
        /// - Events sent will have no observers (silent operation)
        /// - Observer objects themselves are not affected (just unsubscribed)
        /// </summary>
        void RemoveAll() { m_observers.clear(); }

    private:
        /// <summary>
        /// Internal storage for observer subscriptions organized by event type.
        /// 
        /// This map provides efficient lookup of observers by event ID while allowing
        /// multiple observers per event type. The data structure choice reflects the
        /// expected usage patterns:
        /// 
        /// Key Design Decisions:
        /// - std::map: Provides O(log n) lookup by event ID with sorted storage
        /// - std::list: Allows efficient insertion/removal of observers without iterator invalidation
        /// - IObserver*: Stores pointers for efficient copying and minimal memory overhead
        /// 
        /// Structure: Event ID -> List of Observer Pointers
        /// - Each event type maps to a list of interested observers
        /// - Multiple observers can subscribe to the same event type
        /// - Observer order within lists is based on registration order
        /// 
        /// Memory Characteristics:
        /// - Map grows with the number of unique event types used
        /// - Lists grow with the number of observers per event type
        /// - Minimal memory overhead (just pointers, no object copying)
        /// - Automatic cleanup when observers are removed
        /// 
        /// Performance Implications:
        /// - Event dispatch: O(log n + m) where n = event types, m = observers for specific type
        /// - Observer addition: O(log n + 1) for map lookup and list insertion
        /// - Observer removal: O(n * m) for complete removal across all event types
        /// 
        /// Thread Safety:
        /// The current implementation is not inherently thread-safe. Concurrent
        /// access to this data structure may require additional synchronization
        /// mechanisms depending on usage patterns.
        /// 
        /// Alternative Designs:
        /// - std::unordered_map: Faster lookup but higher memory overhead
        /// - std::vector: Faster iteration but slower insertion/removal
        /// - Custom event ID types: Integer IDs for faster comparison
        /// </summary>
        std::map<Event::id_t, std::list<IObserver*>> m_observers;
    };
}