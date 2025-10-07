// ============================================================================
// Object.h
// ============================================================================
#pragma once
#include "Core/Serializable.h"
#include <string>

/// <summary>
/// Convenience macro for implementing the Clone() method in derived classes.
/// 
/// This macro generates the standard Clone() implementation using the copy constructor
/// of the derived class. It eliminates boilerplate code and ensures consistent
/// cloning behavior across all Object-derived classes.
/// 
/// The macro creates a virtual Clone() method that:
/// 1. Uses the copy constructor of the specified class
/// 2. Wraps the result in a std::unique_ptr for automatic memory management
/// 3. Returns the pointer as the base Object type for polymorphic usage
/// 
/// Usage: Place this macro in the public section of derived classes
/// 
/// Example:
/// ```cpp
/// class Player : public Object {
/// public:
///     CLASS_PROTOTYPE(Player)  // Generates Clone() method
///     
///     // Copy constructor is required for proper cloning
///     Player(const Player& other) = default;
/// };
/// ```
/// 
/// Requirements:
/// - The class must have a proper copy constructor (default or custom)
/// - All member variables should be copyable or have custom copy logic
/// - Use this macro for classes that need prototype/cloning functionality
/// </summary>
/// <param name="classname">The name of the derived class implementing Clone()</param>
#define CLASS_PROTOTYPE(classname) virtual std::unique_ptr<Object> Clone() { return std::make_unique<classname>(*this); }

namespace neu {
    /// <summary>
    /// Abstract base class for all game objects that require serialization and cloning capabilities.
    /// 
    /// Object serves as the foundation for the engine's object hierarchy, providing:
    /// - Serialization/deserialization through ISerializable interface
    /// - Prototype pattern support through virtual Clone() method
    /// - Common object properties (name, active state)
    /// - Standardized object lifecycle management
    /// 
    /// Design Philosophy:
    /// - Provides a common interface for all engine objects
    /// - Enables data-driven object creation from serialized configuration
    /// - Supports the Prototype pattern for efficient object duplication
    /// - Maintains consistent object identification and state management
    /// 
    /// Class Hierarchy:
    /// Object is the base class for major engine types:
    /// - Actor: Game entities with transform and component systems
    /// - Component: Modular functionality attached to Actors
    /// - Resource: Loadable assets like textures, sounds, models
    /// - Scene: Collections of Actors and environment data
    /// 
    /// Common Usage Patterns:
    /// 1. **Configuration Loading**: Objects populate themselves from configuration files
    /// 2. **Factory Creation**: Factory systems create objects and load their data
    /// 3. **Prototype Cloning**: Efficient object duplication for spawning
    /// 4. **Save/Load Systems**: Persistent object state management
    /// 
    /// Example Configuration Structure:
    /// ```json
    /// {
    ///     "name": "PlayerCharacter",
    ///     "active": true,
    ///     "components": [...],
    ///     "transform": {...}
    /// }
    /// ```
    /// </summary>
    class Object : public ISerializable {
    public:
        /// <summary>
        /// Human-readable identifier for the object.
        /// 
        /// The name serves multiple purposes:
        /// - Object identification in debugging and logging
        /// - Asset referencing in configuration files
        /// - Scene graph navigation and object lookup
        /// - User interface display in editors and debug tools
        /// 
        /// Naming conventions:
        /// - Use descriptive names that indicate object purpose
        /// - Consider hierarchical naming for related objects (e.g., "Player.Weapon.Sword")
        /// - Avoid special characters that might interfere with file systems or parsing
        /// 
        /// Examples: "MainCamera", "PlayerCharacter", "Background_Layer1", "EnemySpawner"
        /// </summary>
        std::string name;

        /// <summary>
        /// Boolean flag indicating whether the object is currently active/enabled.
        /// 
        /// The active state controls object participation in game systems:
        /// - Active objects participate in update loops, rendering, and collision detection
        /// - Inactive objects are typically skipped during processing for performance
        /// - State can be toggled at runtime for dynamic show/hide functionality
        /// - Useful for implementing object pooling and temporary disabling
        /// 
        /// Default value is true, meaning objects are active upon creation.
        /// 
        /// Usage patterns:
        /// - Temporary object disabling without destruction
        /// - Implementing visibility toggles and conditional behavior
        /// - Object pooling systems that reuse inactive objects
        /// - Pause/unpause functionality for specific objects
        /// </summary>
        bool active{ true };

    public:
        /// <summary>
        /// Default constructor initializing object with default values.
        /// 
        /// Creates an Object with:
        /// - Empty name string (should be set through Read() or manually)
        /// - Active state set to true
        /// - Ready for configuration through serialization
        /// 
        /// Derived classes should call this constructor and perform additional
        /// initialization in their own constructors or Start() methods.
        /// </summary>
        Object() = default;

        /// <summary>
        /// Virtual destructor ensuring proper cleanup of derived class objects.
        /// 
        /// Essential for abstract base classes to guarantee that derived class
        /// destructors are called when objects are deleted through base class pointers.
        /// This prevents resource leaks and ensures proper cleanup of derived class resources.
        /// 
        /// The virtual destructor enables polymorphic destruction, allowing:
        /// - Safe deletion through Object* pointers
        /// - Proper cleanup of derived class members
        /// - Exception-safe resource management through RAII
        /// </summary>
        virtual ~Object() = default;

        /// <summary>
        /// Virtual initialization method called after object creation and configuration.
        /// 
        /// This method provides a standardized initialization hook for all Object-derived
        /// classes, called after the object is fully constructed and configured. It allows
        /// objects to perform setup operations that require access to their complete state
        /// or external context (like scene references).
        /// 
        /// Initialization timing:
        /// 1. Object is constructed (constructor called)
        /// 2. Object is configured (Read() called for serialized objects)
        /// 3. Object is added to its container (Scene, Actor, etc.)
        /// 4. Start() is called for final initialization
        /// 
        /// The base implementation returns true by default, indicating successful
        /// initialization. Derived classes can override this to perform specific
        /// setup operations and return false if initialization fails.
        /// 
        /// Common initialization tasks:
        /// - Caching references to related objects or systems
        /// - Setting up event listeners or observers
        /// - Validating object state and configuration
        /// - Performing one-time setup that requires external context
        /// - Initializing resources that depend on complete object state
        /// 
        /// Example overrides:
        /// ```cpp
        /// // Actor initialization
        /// bool Actor::Start() override {
        ///     if (!Object::Start()) return false;
        ///     for (auto& component : m_components) {
        ///         if (!component->Start()) return false;
        ///     }
        ///     return true;
        /// }
        /// 
        /// // Component initialization  
        /// bool AudioComponent::Start() override {
        ///     if (!Object::Start()) return false;
        ///     m_audioSource = AudioManager::CreateSource();
        ///     return m_audioSource != nullptr;
        /// }
        /// ```
        /// 
        /// Return value semantics:
        /// - true: Initialization successful, object ready for use
        /// - false: Initialization failed, object should not be used
        /// 
        /// Error handling:
        /// - Failed initialization should be logged appropriately
        /// - Objects that fail to start may be removed or disabled
        /// - Always call base class Start() first in overrides
        /// </summary>
        /// <returns>True if initialization succeeded, false if it failed</returns>
        virtual bool Start() { return true; }

        /// <summary>
        /// Virtual cleanup method called before object destruction.
        /// 
        /// This method provides a standardized cleanup hook for all Object-derived
        /// classes, called before the object is actually destroyed. It gives objects
        /// an opportunity to perform cleanup operations while they are still in a
        /// valid state and can access their full interface.
        /// 
        /// Cleanup timing:
        /// 1. Object is marked for destruction or removal decision is made
        /// 2. Destroyed() is called while object is still valid
        /// 3. Object is removed from containers
        /// 4. Actual destructor is called and memory is freed
        /// 
        /// The base implementation is empty, providing a default no-op cleanup.
        /// Derived classes override this to perform specific cleanup operations
        /// relevant to their functionality and resources.
        /// 
        /// Common cleanup tasks:
        /// - Releasing allocated resources (memory, file handles, network connections)
        /// - Unregistering from event systems or observer patterns
        /// - Notifying other objects of this object's destruction
        /// - Saving persistent state or important data
        /// - Cleaning up references that might prevent garbage collection
        /// - Stopping ongoing operations (animations, network requests, timers)
        /// 
        /// Example overrides:
        /// ```cpp
        /// // Actor cleanup
        /// void Actor::Destroyed() override {
        ///     for (auto& component : m_components) {
        ///         component->Destroyed();
        ///     }
        ///     Object::Destroyed();
        /// }
        /// 
        /// // Component cleanup
        /// void AudioComponent::Destroyed() override {
        ///     if (m_audioSource) {
        ///         m_audioSource->Stop();
        ///         AudioManager::ReleaseSource(m_audioSource);
        ///     }
        ///     Object::Destroyed();
        /// }
        /// ```
        /// 
        /// Important notes:
        /// - This is NOT the destructor - the object is still valid during this call
        /// - Avoid operations that might throw exceptions during cleanup
        /// - Don't rely on other objects still being valid (they might be destroyed too)
        /// - This method should be exception-safe and robust
        /// - Consider calling base class Destroyed() at the end of overrides
        /// </summary>
        virtual void Destroyed() {}

        /// <summary>
        /// Pure virtual method for creating a deep copy of the object.
        /// 
        /// This method implements the Prototype pattern, enabling efficient object duplication
        /// without requiring knowledge of the concrete object type. Each derived class must
        /// provide its own implementation that properly copies all relevant state.
        /// 
        /// The Clone() method should:
        /// - Create a new instance of the same concrete type
        /// - Copy all member variables and state information
        /// - Handle deep copying of complex members (pointers, containers)
        /// - Maintain object integrity and invariants
        /// - Return ownership through std::unique_ptr for memory safety
        /// 
        /// Implementation approaches:
        /// 1. **Macro approach**: Use CLASS_PROTOTYPE(ClassName) for simple cases
        /// 2. **Custom approach**: Implement manually for complex copying logic
        /// 3. **Copy constructor**: Ensure proper copy constructor exists for macro usage
        /// 
        /// Use cases:
        /// - Object spawning systems (bullets, enemies, particles)
        /// - Template-based object creation
        /// - Save/load systems with object duplication
        /// - Undo/redo functionality requiring object snapshots
        /// 
        /// Example implementations:
        /// ```cpp
        /// // Simple case using macro
        /// class SimpleObject : public Object {
        /// public:
        ///     CLASS_PROTOTYPE(SimpleObject)
        /// };
        /// 
        /// // Complex case with custom logic
        /// class ComplexObject : public Object {
        /// public:
        ///     std::unique_ptr&lt;Object&gt; Clone() override {
        ///         auto clone = std::make_unique&lt;ComplexObject&gt;();
        ///         clone-&gt;name = this-&gt;name;
        ///         clone-&gt;active = this-&gt;active;
        ///         // Custom deep copying logic here
        ///         return clone;
        ///     }
        /// };
        /// ```
        /// </summary>
        /// <returns>A unique_ptr to a new Object instance that is a deep copy of this object</returns>
        virtual std::unique_ptr<Object> Clone() = 0;

        /// <summary>
        /// Deserializes object state from serialized data.
        /// 
        /// This method implements the ISerializable interface, providing standardized
        /// deserialization for all Object-derived classes. The base implementation
        /// handles common Object properties, while derived classes can override to
        /// add their specific deserialization logic.
        /// 
        /// Base Object properties loaded:
        /// - "name": String identifier for the object
        /// - "active": Boolean flag for object active state
        /// 
        /// The method uses serialization utility macros for safe data extraction with default values,
        /// ensuring graceful handling of missing or malformed data.
        /// 
        /// Derived classes should override this method to:
        /// 1. Call the base class implementation first: Object::Read(value)
        /// 2. Load their specific properties using SERIAL_READ macros
        /// 3. Perform any post-loading initialization or validation
        /// 4. Handle backwards compatibility for format changes
        /// 
        /// Example override:
        /// ```cpp
        /// void Player::Read(const serial_data_t& value) override {
        ///     Object::Read(value);  // Load base properties
        ///     SERIAL_READ(value, health);
        ///     SERIAL_READ(value, level);
        ///     SERIAL_READ(value, position);
        /// }
        /// ```
        /// 
        /// Error handling:
        /// - Missing properties use default values
        /// - Malformed data is logged but doesn't prevent loading
        /// - Critical errors may require custom validation logic
        /// </summary>
        /// <param name="value">Serialized data containing the object's state information</param>
        void Read(const serial_data_t& value) override;
    };
}
