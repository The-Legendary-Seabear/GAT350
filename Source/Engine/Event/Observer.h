#pragma once

namespace neu {
    /// <summary>
    /// Abstract interface implementing the Observer pattern for event-driven notifications.
    /// 
    /// The IObserver interface defines the contract for objects that want to receive
    /// notifications when events occur in the system. This implements the classic Observer
    /// pattern, enabling loose coupling between event producers (subjects) and event
    /// consumers (observers).
    /// 
    /// Design Pattern Implementation:
    /// - Observer Pattern: Objects register to receive notifications about events
    /// - Event-driven architecture: Observers react to events without tight coupling
    /// - Polymorphic dispatch: Different observer types can handle events differently
    /// - Registration system: Observers subscribe/unsubscribe from event sources
    /// 
    /// Benefits of Observer Pattern:
    /// - Decoupled communication: Event sources don't need to know about specific observers
    /// - Dynamic relationships: Observers can be added or removed at runtime
    /// - Multiple observers: Many objects can observe the same events simultaneously
    /// - Event filtering: Observers can choose which events to respond to
    /// - Clean separation: Business logic separated from notification logic
    /// 
    /// Common Use Cases:
    /// - UI updates responding to game state changes
    /// - Audio system responding to game events (footsteps, explosions, music triggers)
    /// - Achievement system monitoring player actions
    /// - Statistics tracking responding to gameplay events
    /// - AI systems reacting to environmental changes
    /// - Logging systems recording events for debugging
    /// - Network systems synchronizing state changes
    /// 
    /// Integration with Event System:
    /// This interface works together with Event and EventManager classes to provide
    /// a complete event-driven architecture. Typical workflow:
    /// 1. Objects implement IObserver to receive event notifications
    /// 2. Observers register with EventManager for specific event types
    /// 3. Event producers send events through EventManager
    /// 4. EventManager dispatches events to all registered observers
    /// 5. Each observer's OnNotify method is called with the event data
    /// 
    /// Example Implementation:
    /// ```cpp
    /// class HealthBar : public IObserver {
    /// public:
    ///     void OnNotify(const Event& event) override {
    ///         if (event.id == "player_health_changed") {
    ///             if (std::holds_alternative&lt;int&gt;(event.data)) {
    ///                 int newHealth = std::get&lt;int&gt;(event.data);
    ///                 UpdateHealthDisplay(newHealth);
    ///             }
    ///         }
    ///     }
    /// };
    /// ```
    /// 
    /// Performance Considerations:
    /// - OnNotify may be called frequently, so implementations should be efficient
    /// - Consider filtering events early to avoid unnecessary processing
    /// - Use event IDs for quick event type identification
    /// - Avoid heavy computations in notification handlers
    /// - Consider deferring expensive operations to later frames
    /// </summary>
    class IObserver {
    public:
        /// <summary>
        /// Virtual destructor ensuring proper cleanup of derived observer classes.
        /// 
        /// Essential for abstract base classes to guarantee that derived class destructors
        /// are called when observers are deleted through base class pointers. This is
        /// particularly important in observer systems where observers may be managed
        /// through IObserver pointers for polymorphic behavior.
        /// 
        /// The destructor also provides an opportunity for observers to automatically
        /// unregister themselves from event managers during cleanup, preventing
        /// dangling pointer issues and ensuring clean shutdown.
        /// 
        /// Best Practice:
        /// Derived classes should use their destructors to unregister from any
        /// event managers they're subscribed to, ensuring proper cleanup:
        /// ```cpp
        /// ~MyObserver() {
        ///     EventManager::Instance().Unregister(this);
        /// }
        /// ```
        /// </summary>
        virtual ~IObserver();

        /// <summary>
        /// Pure virtual method called when an observed event occurs.
        /// 
        /// This method must be implemented by all concrete observer classes and serves
        /// as the primary entry point for event notification. When an event is dispatched
        /// through the event system, this method is called on all registered observers.
        /// 
        /// Implementation Guidelines:
        /// - Check the event ID to determine if this observer should respond
        /// - Use std::holds_alternative to verify expected data types
        /// - Extract event data safely using std::get or std::visit
        /// - Keep processing efficient as this may be called frequently
        /// - Handle unexpected event types gracefully (ignore or log)
        /// - Avoid modifying the event object (it may be shared)
        /// 
        /// Event Filtering Strategies:
        /// 1. **ID-based filtering**: Check event.id for relevant event types
        /// 2. **Data-type filtering**: Use std::holds_alternative to check data types
        /// 3. **Registration filtering**: Register only for specific event types
        /// 4. **Conditional filtering**: Check additional conditions before processing
        /// 
        /// Error Handling:
        /// - Validate event data before using it
        /// - Handle type mismatches gracefully
        /// - Log unexpected events for debugging
        /// - Avoid throwing exceptions that might disrupt event dispatch
        /// 
        /// Example Implementations:
        /// ```cpp
        /// // Simple event filtering
        /// void AudioManager::OnNotify(const Event& event) override {
        ///     if (event.id == "play_sound") {
        ///         if (std::holds_alternative&lt;std::string&gt;(event.data)) {
        ///             std::string soundName = std::get&lt;std::string&gt;(event.data);
        ///             PlaySound(soundName);
        ///         }
        ///     }
        /// }
        /// 
        /// // Multiple event type handling
        /// void GameUI::OnNotify(const Event& event) override {
        ///     if (event.id == "player_health_changed") {
        ///         UpdateHealthBar(std::get&lt;int&gt;(event.data));
        ///     } else if (event.id == "player_score_changed") {
        ///         UpdateScoreDisplay(std::get&lt;int&gt;(event.data));
        ///     } else if (event.id == "game_paused") {
        ///         ShowPauseMenu(std::get&lt;bool&gt;(event.data));
        ///     }
        /// }
        /// 
        /// // Using std::visit for type-safe processing
        /// void Logger::OnNotify(const Event& event) override {
        ///     std::visit([&](const auto& value) {
        ///         LogEvent(event.id, value);
        ///     }, event.data);
        /// }
        /// ```
        /// 
        /// Threading Considerations:
        /// - This method may be called from different threads depending on event system design
        /// - Ensure thread-safe access to shared state if applicable
        /// - Consider using thread-safe event queues for cross-thread notifications
        /// - Avoid blocking operations that might stall event dispatch
        /// 
        /// Performance Notes:
        /// - Early exit for irrelevant events to minimize processing overhead
        /// - Cache frequently used data to avoid repeated lookups
        /// - Consider batching related operations if multiple events arrive
        /// - Profile notification handlers in performance-critical systems
        /// </summary>
        /// <param name="event">
        /// Const reference to the event being notified about.
        /// The event contains both an ID identifying the event type and variant data
        /// containing event-specific information. The event should not be modified
        /// as it may be dispatched to multiple observers.
        /// 
        /// The event reference is valid only during the OnNotify call. If the observer
        /// needs to store event data for later processing, it should copy the relevant
        /// information rather than storing the event reference.
        /// </param>
        virtual void OnNotify(const Event& event) = 0;
    };
}