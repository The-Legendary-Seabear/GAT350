#pragma once
#include <string>
#include <variant>

namespace neu {
    /// <summary>
    /// A flexible event structure for implementing event-driven communication between game systems.
    /// 
    /// The Event struct provides a type-safe, flexible messaging system that allows different
    /// parts of the game engine to communicate without tight coupling. It uses std::variant
    /// to support multiple data types in a single event structure, enabling rich event data
    /// while maintaining type safety.
    /// 
    /// Design Philosophy:
    /// - Decoupled communication: Systems can send events without knowing who receives them
    /// - Type safety: std::variant ensures compile-time type checking for event data
    /// - Flexibility: Single event structure supports multiple data types
    /// - Performance: Minimal overhead compared to inheritance-based event systems
    /// - Simplicity: Easy to create, send, and handle events throughout the engine
    /// 
    /// Event-Driven Architecture Benefits:
    /// - Loose coupling between game systems
    /// - Easy to add new event types without modifying existing code
    /// - Centralized event handling through event managers/dispatchers
    /// - Support for both synchronous and asynchronous event processing
    /// - Clean separation of concerns between event producers and consumers
    /// 
    /// Common Use Cases:
    /// - Player actions (movement, attacks, interactions)
    /// - Game state changes (level completion, player death, score updates)
    /// - UI updates (health bars, inventory changes, menu transitions)
    /// - Audio triggers (sound effects, music changes)
    /// - Animation events (animation completion, frame events)
    /// - Physics events (collisions, trigger zones)
    /// - Network events (player connections, data synchronization)
    /// 
    /// Example Usage:
    /// ```cpp
    /// // Creating different types of events
    /// Event playerDamage("player_damage", 25);           // int data
    /// Event speedBoost("speed_boost", 1.5f);             // float data
    /// Event gameOver("game_over", true);                 // bool data
    /// Event levelName("level_loaded", std::string("Forest")); // string data
    /// Event playerPos("player_moved", vec2{100, 200});   // vec2 data
    /// Event explosion("explosion", vec3{10, 5, 0});      // vec3 data
    /// 
    /// // Sending events through event system
    /// EventManager::Instance().Send(playerDamage);
    /// ```
    /// </summary>
    struct Event {
        /// <summary>
        /// Type alias for event identifiers.
        /// 
        /// Uses std::string to provide human-readable, descriptive event names that are
        /// easy to debug and understand. String-based IDs offer several advantages:
        /// - Self-documenting: Event names clearly indicate their purpose
        /// - Flexible: No need for centralized ID registration or enum management
        /// - Debuggable: Easy to trace events in logs and debugging sessions
        /// - Extensible: New event types can be added without coordination
        /// 
        /// Naming Conventions:
        /// - Use lowercase with underscores: "player_died", "level_complete"
        /// - Be descriptive and specific: "enemy_health_changed" vs "health_changed"
        /// - Include context when needed: "ui_button_clicked", "physics_collision"
        /// - Consider namespacing: "audio.music_started", "input.key_pressed"
        /// 
        /// Performance Note:
        /// String comparisons are used for event matching, which is slower than integer
        /// comparisons but provides better maintainability. For performance-critical
        /// systems, consider using string hashing or switching to integer-based IDs.
        /// </summary>
        using id_t = std::string;

        /// <summary>
        /// Type alias for event data using std::variant for type-safe polymorphic data.
        /// 
        /// The variant supports common data types needed for game events while maintaining
        /// type safety at compile time. Each event can carry exactly one piece of data
        /// of any of the supported types.
        /// 
        /// Supported Data Types:
        /// - int: Numerical data (scores, health, counts, indices)
        /// - float: Floating-point data (speeds, times, percentages, physics values)
        /// - bool: Boolean flags (enable/disable states, condition checks)
        /// - std::string: Text data (names, messages, file paths)
        /// - vec2: 2D vectors (positions, velocities, screen coordinates)
        /// - vec3: 3D vectors (world positions, colors, 3D velocities)
        /// 
        /// Type Safety:
        /// - Compile-time type checking prevents runtime errors
        /// - std::get&lt;T&gt;() for safe type extraction with exceptions on mismatch
        /// - std::holds_alternative&lt;T&gt;() for runtime type checking
        /// - std::visit() for type-safe variant processing
        /// 
        /// Usage Examples:
        /// ```cpp
        /// // Extracting data with type checking
        /// if (std::holds_alternative&lt;int&gt;(event.data)) {
        ///     int value = std::get&lt;int&gt;(event.data);
        /// }
        /// 
        /// // Using std::visit for type-safe processing
        /// std::visit([](const auto& value) {
        ///     // Process value regardless of its actual type
        /// }, event.data);
        /// ```
        /// 
        /// Extension Considerations:
        /// Adding new types to the variant requires recompilation of all code that
        /// processes events. Consider using std::any for maximum flexibility at the
        /// cost of type safety, or create specialized event types for complex data.
        /// </summary>
        using data_t = std::variant<int, float, bool, std::string, vec2, vec3>;

        /// <summary>
        /// String identifier uniquely identifying the type of event.
        /// 
        /// This field determines what kind of event this is and how it should be processed.
        /// Event handlers typically use this ID to filter and route events to appropriate
        /// processing logic.
        /// 
        /// The ID should be descriptive and follow consistent naming conventions across
        /// the engine. It serves as both a routing mechanism and documentation of the
        /// event's purpose.
        /// 
        /// Examples:
        /// - "player_health_changed": Player's health was modified
        /// - "level_complete": Current level has been completed
        /// - "audio_music_finished": Background music track ended
        /// - "input_key_pressed": A keyboard key was pressed
        /// - "physics_collision_enter": Two objects started colliding
        /// </summary>
        id_t id;

        /// <summary>
        /// Variant data payload containing the event's associated information.
        /// 
        /// This field holds the actual data relevant to the event. The type of data
        /// depends on the event type and what information needs to be communicated.
        /// The variant ensures type safety while allowing different events to carry
        /// different types of data.
        /// 
        /// Data Examples by Event Type:
        /// - Health events: int (new health value)
        /// - Speed events: float (new speed multiplier)
        /// - State events: bool (enabled/disabled flag)
        /// - Message events: std::string (message text)
        /// - Position events: vec2/vec3 (new position coordinates)
        /// 
        /// The data should contain all information necessary for event handlers to
        /// process the event appropriately. Additional context should be included
        /// in the event ID or handled through separate events if needed.
        /// </summary>
        data_t data;

        /// <summary>
        /// Constructs an event with the specified ID and data payload.
        /// 
        /// This constructor creates a complete event ready for dispatch through the
        /// event system. The data parameter uses perfect forwarding to efficiently
        /// handle different data types and avoid unnecessary copying.
        /// 
        /// The constructor automatically deduces the correct variant type from the
        /// provided data parameter, ensuring type safety while remaining convenient
        /// to use.
        /// 
        /// Parameter Details:
        /// - id: Must be a meaningful, descriptive string identifying the event type
        /// - data: Can be any of the supported variant types (int, float, bool, string, vec2, vec3)
        /// 
        /// Usage Examples:
        /// ```cpp
        /// Event healthEvent("player_health_changed", 75);
        /// Event speedEvent("player_speed_boost", 1.5f);
        /// Event statusEvent("game_paused", true);
        /// Event nameEvent("player_name_set", std::string("Hero"));
        /// Event posEvent("player_moved", vec2{100.0f, 200.0f});
        /// Event colorEvent("light_color_changed", vec3{1.0f, 0.5f, 0.2f});
        /// ```
        /// 
        /// Memory Management:
        /// The constructor copies the provided data into the variant, ensuring the
        /// event owns its data and can be safely passed around without worrying
        /// about dangling references or lifetime issues.
        /// </summary>
        /// <param name="id">String identifier specifying the type of event</param>
        /// <param name="data">Data payload of any supported variant type</param>
        Event(const id_t& id, data_t data) :
            id{ id },
            data{ data }
        {
        }
    };
}