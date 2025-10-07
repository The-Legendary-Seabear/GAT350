#pragma once
#include "Object.h"

namespace neu {
    /// <summary>
    /// Abstract base class for all components in the Entity-Component-System (ECS) architecture.
    /// 
    /// Components represent modular pieces of functionality that can be attached to Actor entities
    /// to provide specific behaviors, data, or capabilities. This design promotes composition over
    /// inheritance, allowing flexible and reusable game object construction.
    /// 
    /// Key Design Principles:
    /// - Composition over inheritance: Actors gain functionality through attached components
    /// - Single responsibility: Each component should handle one specific aspect of behavior
    /// - Data-driven design: Components can be configured and instantiated from external data
    /// - Lifecycle management: Automatic initialization and cleanup through virtual methods
    /// 
    /// Component Lifecycle:
    /// 1. Construction: Component is created and attached to an Actor
    /// 2. Start(): Called when the component becomes active (initialization logic)
    /// 3. Update(): Called every frame while the component is active
    /// 4. Destroyed(): Called before the component is removed (cleanup logic)
    /// 5. Destruction: Component is destroyed and memory is freed
    /// 
    /// Common Component Types:
    /// - Transform: Position, rotation, and scale data
    /// - Renderer: Visual representation and drawing logic
    /// - Physics: Collision detection and physics simulation
    /// - Audio: Sound effects and music playback
    /// - AI: Artificial intelligence behaviors
    /// - Animation: Sprite and model animation systems
    /// - Input: Player input handling and response
    /// 
    /// Example Implementation:
    /// ```cpp
    /// class HealthComponent : public Component {
    /// public:
    ///     void Start() override {
    ///         m_maxHealth = 100;
    ///         m_currentHealth = m_maxHealth;
    ///     }
    ///     
    ///     void Update(float dt) override {
    ///         // Handle health regeneration, damage over time, etc.
    ///         if (m_isRegenerating) {
    ///             m_currentHealth = std::min(m_maxHealth, m_currentHealth + m_regenRate * dt);
    ///         }
    ///     }
    ///     
    ///     void TakeDamage(float damage) {
    ///         m_currentHealth = std::max(0.0f, m_currentHealth - damage);
    ///         if (m_currentHealth <= 0) {
    ///             // Notify owner of death
    ///         }
    ///     }
    /// };
    /// ```
    /// </summary>
    class Component : public Object {
    public:
        /// <summary>
        /// Pointer to the Actor that owns this component.
        /// 
        /// This provides bidirectional communication between the component and its parent Actor.
        /// The owner pointer allows components to:
        /// - Access other components on the same Actor
        /// - Query Actor properties (position, name, etc.)
        /// - Send messages or events to the Actor
        /// - Request component additions or removals
        /// 
        /// The pointer is set automatically when the component is attached to an Actor
        /// and should not be modified manually. It uses a forward declaration to avoid
        /// circular dependencies between Component.h and Actor.h.
        /// 
        /// Usage examples:
        /// - owner->GetComponent&lt;TransformComponent&gt;() // Access sibling components
        /// - owner->RemoveComponent(this) // Self-removal
        /// - owner->SendMessage("ComponentReady") // Actor communication
        /// 
        /// Note: This pointer may be null during construction or after the Actor is destroyed.
        /// Always verify validity before dereferencing in critical code paths.
        /// </summary>
        class Actor* owner{ nullptr };

    public:
        /// <summary>
        /// Default constructor for component initialization.
        /// 
        /// Components start in an unattached state with no owner. The owner pointer
        /// will be set when the component is attached to an Actor through the
        /// Actor's component management system.
        /// 
        /// Derived classes should use their constructors for basic initialization
        /// and defer complex setup logic to the Start() method, which is called
        /// after the component is properly attached and the game world is ready.
        /// </summary>
        Component() = default;
        
        /// <summary>
        /// Pure virtual update method called every frame while the component is active.
        /// 
        /// This is the main execution method for components, called once per frame
        /// during the game's update loop. All component logic that needs to run
        /// continuously should be implemented here.
        /// 
        /// The method must be implemented by all concrete component classes, as it
        /// defines the core behavior and functionality of the component.
        /// 
        /// Common update operations include:
        /// - Updating animation states and timers
        /// - Processing physics or movement calculations
        /// - Handling state transitions and game logic
        /// - Responding to input or external events
        /// - Updating visual effects and particle systems
        /// - Managing AI behavior and decision making
        /// 
        /// Performance considerations:
        /// - Keep update logic efficient as this is called every frame
        /// - Consider using fixed timestep updates for physics-heavy components
        /// - Cache expensive calculations when possible
        /// - Use early exits to avoid unnecessary work
        /// - Profile update methods in performance-critical components
        /// 
        /// Frame-rate independence:
        /// - Use the provided delta time parameter for time-based calculations
        /// - Scale movements, animations, and timers by dt for consistent behavior
        /// - Avoid frame-dependent logic that assumes a specific frame rate
        /// 
        /// Example implementations:
        /// ```cpp
        /// // Movement component
        /// void MovementComponent::Update(float dt) override {
        ///     if (owner && m_velocity.length() > 0) {
        ///         auto transform = owner->GetComponent&lt;TransformComponent&gt;();
        ///         if (transform) {
        ///             transform->position += m_velocity * dt;
        ///         }
        ///     }
        /// }
        /// 
        /// // Timer component
        /// void TimerComponent::Update(float dt) override {
        ///     m_elapsed += dt;
        ///     if (m_elapsed >= m_duration) {
        ///         TriggerEvent();
        ///         m_elapsed = 0.0f;
        ///     }
        /// }
        /// ```
        /// </summary>
        /// <param name="dt">
        /// Delta time in seconds since the last frame update.
        /// This value represents the time elapsed between the current frame and the previous frame,
        /// providing the foundation for frame-rate independent calculations. Use this parameter
        /// to scale time-dependent operations such as movement, animation, and timers.
        /// 
        /// The delta time is typically a small positive value (e.g., 0.016 seconds for 60 FPS),
        /// but can vary based on system performance and frame rate fluctuations.
        /// </param>
        virtual void Update(float dt) = 0;
    };
}