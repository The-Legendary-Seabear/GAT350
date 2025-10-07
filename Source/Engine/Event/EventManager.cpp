#include "EnginePCH.h"
#include "Core/StringHelper.h"

namespace neu {
    /// <summary>
    /// Registers an observer to receive notifications for a specific event type.
    /// 
    /// This method adds the observer to the subscription list for the given event ID.
    /// The event ID is converted to lowercase to ensure case-insensitive event handling,
    /// preventing registration/notification mismatches due to inconsistent casing.
    /// 
    /// Implementation details:
    /// - Uses map's operator[] which creates an entry if it doesn't exist
    /// - Stores observer as a pointer for efficient copying and storage
    /// - Allows duplicate registrations (same observer can register multiple times for same event)
    /// - Registration is immediate and takes effect for the next event dispatch
    /// 
    /// Memory management:
    /// - The observer object must remain valid until it's unregistered
    /// - EventManager does not take ownership of the observer
    /// - No validation is performed on the observer reference
    /// </summary>
    /// <param name="id">String identifier for the event type (converted to lowercase internally)</param>
    /// <param name="observer">Reference to the observer that will receive notifications</param>
    void EventManager::AddObserver(const Event::id_t& id, IObserver& observer) {
        // Convert event ID to lowercase for case-insensitive event handling
        // This ensures "PlayerDied", "playerdied", and "PLAYERDIED" all map to the same event
        // operator[] creates a new list if the event type doesn't exist
        m_observers[toLower(id)].push_back(&observer);
    }

    /// <summary>
    /// Removes an observer from all event subscriptions and cleans up empty event type entries.
    /// 
    /// This method performs a complete unregistration of the observer across all event types.
    /// It uses an iterator-based approach to safely remove empty event type entries during
    /// iteration, preventing memory bloat from unused event type mappings.
    /// 
    /// Algorithm:
    /// 1. Iterate through all event type mappings
    /// 2. For each event type, remove all instances of the observer
    /// 3. If the observer list becomes empty, remove the entire event type entry
    /// 4. Use careful iterator management to handle container modification during iteration
    /// 
    /// Performance characteristics:
    /// - O(n * m) where n = number of event types, m = average observers per type
    /// - std::erase_if provides efficient removal with single pass through each list
    /// - Empty event type cleanup prevents map from growing indefinitely
    /// 
    /// The cleanup is important because without it, the map would accumulate empty
    /// entries over time, consuming memory and slightly degrading lookup performance.
    /// </summary>
    /// <param name="observer">Reference to the observer to remove from all subscriptions</param>
    void EventManager::RemoveObserver(IObserver& observer) {
        // Iterate through all event types with manual iterator control
        // This allows safe removal of map entries during iteration
        for (auto it = m_observers.begin(); it != m_observers.end();) {
            // Get reference to the observer list for this event type
            auto& observers = it->second;

            // Remove all instances of this observer from the current event type
            // std::erase_if efficiently removes matching elements in a single pass
            // Lambda captures observer by reference and compares pointer addresses
            std::erase_if(observers, [&observer](auto obs) {
                return obs == &observer;
                });

            // Clean up empty event type entries to prevent memory bloat
            // If no observers remain for this event type, remove the entire entry
            if (observers.empty()) {
                // erase() returns iterator to the next element
                it = m_observers.erase(it);
            }
            else {
                // Only increment iterator if we didn't erase the current element
                ++it;
            }
        }
    }

    /// <summary>
    /// Dispatches an event to all observers registered for its event type.
    /// 
    /// This method implements synchronous event delivery, immediately notifying all
    /// registered observers for the given event type. The event ID is converted to
    /// lowercase to match the case-insensitive registration system.
    /// 
    /// Dispatch behavior:
    /// - Events with no registered observers are silently ignored (this is normal)
    /// - All observers are notified in registration order (list iteration order)
    /// - Notification is synchronous - all observers are called before method returns
    /// - Observer exceptions may propagate and stop further notifications
    /// 
    /// Performance:
    /// - O(log n + m) where n = number of event types, m = observers for this event
    /// - Map lookup is O(log n) for event type
    /// - Observer notification is O(m) for direct function calls
    /// - No dynamic memory allocation during dispatch
    /// 
    /// Error handling:
    /// - Events with unrecognized IDs are silently ignored (no error condition)
    /// - Observer exceptions are not caught (may interrupt dispatch to remaining observers)
    /// - Invalid observer pointers would cause undefined behavior
    /// </summary>
    /// <param name="event">The event to dispatch, containing ID and data payload</param>
    void EventManager::Notify(const Event& event) {
        // Look up observers for this event type using case-insensitive comparison
        auto it = m_observers.find(toLower(event.id));

        // Only proceed if we have registered observers for this event type
        if (it != m_observers.end()) {
            // Get reference to the list of observers for this event type
            auto& observers = it->second;

            // Notify each observer in registration order
            // Uses range-based for loop for clean, efficient iteration
            for (auto observer : observers) {
                // Call the observer's notification method with the event
                // Note: If observer throws an exception, remaining observers won't be notified
                observer->OnNotify(event);
            }
        }
        // Events with no observers are silently ignored - this is expected behavior
        // Many events may be sent speculatively or for optional features
    }
}
