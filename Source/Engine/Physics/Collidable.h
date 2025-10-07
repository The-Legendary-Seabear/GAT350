#pragma once

namespace neu {
    /// <summary>
    /// Abstract interface for objects that can participate in collision detection and response.
    /// 
    /// This interface defines the contract for objects that need to react to collisions
    /// with other actors in the game world. It enables polymorphic collision handling
    /// where different types of objects can respond to collisions in their own specific ways.
    /// 
    /// Design Philosophy:
    /// - Provides a uniform interface for collision response across different object types
    /// - Enables event-driven collision handling through callback methods
    /// - Separates collision detection (handled by physics/collision systems) from collision response
    /// - Supports flexible collision behavior through inheritance and polymorphism
    /// 
    /// Integration with Physics Systems:
    /// - Physics/collision detection systems identify when collisions occur
    /// - The system calls OnCollision() on objects that implement this interface
    /// - Objects can then implement custom logic for their specific collision responses
    /// - This decouples collision detection algorithms from collision response behaviors
    /// 
    /// Common Use Cases:
    /// - Player characters responding to enemy contact (take damage, knockback)
    /// - Projectiles hitting targets (explode, deal damage, disappear)
    /// - Collectible items being picked up by players
    /// - Environmental triggers activating when players enter areas
    /// - Dynamic objects responding to physics collisions
    /// 
    /// Typical Implementation Pattern:
    /// ```cpp
    /// class Player : public Actor, public ICollidable {
    /// public:
    ///     void OnCollision(Actor* other) override {
    ///         if (auto enemy = dynamic_cast&lt;Enemy*&gt;(other)) {
    ///             TakeDamage(enemy->GetDamage());
    ///         } else if (auto powerup = dynamic_cast&lt;PowerUp*&gt;(other)) {
    ///             CollectPowerUp(powerup);
    ///         }
    ///     }
    /// };
    /// ```
    /// 
    /// Performance Considerations:
    /// - OnCollision() may be called frequently during collision-heavy scenarios
    /// - Implementations should be efficient and avoid expensive operations when possible
    /// - Consider using object pooling for frequently created/destroyed collision objects
    /// - Use type checking (dynamic_cast) judiciously to maintain performance
    /// </summary>
    class ICollidable {
    public:
        /// <summary>
        /// Virtual destructor ensuring proper cleanup of derived collidable objects.
        /// 
        /// Essential for abstract base classes to guarantee that derived class destructors
        /// are called when objects are deleted through base class pointers. This is
        /// particularly important in collision systems where objects may be managed
        /// through ICollidable pointers for polymorphic behavior.
        /// 
        /// The default implementation allows derived classes to provide their own
        /// destructors while ensuring virtual dispatch works correctly during cleanup.
        /// </summary>
        virtual ~ICollidable() = default;

        /// <summary>
        /// Callback method invoked when this object collides with another Actor.
        /// 
        /// This pure virtual method must be implemented by all classes that inherit
        /// from ICollidable. It serves as the primary entry point for collision response
        /// logic and is called by the physics/collision system when a collision is detected.
        /// 
        /// Implementation Guidelines:
        /// - Determine the type of the colliding object and respond appropriately
        /// - Handle collision logic specific to this object's behavior and game role
        /// - Avoid modifying the collision system state from within this callback
        /// - Keep implementations efficient as this may be called frequently
        /// - Consider the collision as mutual - both objects involved will typically receive callbacks
        /// 
        /// Parameter Usage:
        /// - Use dynamic_cast to safely determine the type of the colliding Actor
        /// - Check for null pointers if collision system might pass invalid references
        /// - Access the other Actor's properties, components, or methods as needed
        /// - Coordinate with the other object if needed (e.g., both taking damage)
        /// 
        /// Response Strategies:
        /// 1. **Immediate Response**: Direct state changes, damage, destruction
        /// 2. **Deferred Response**: Queue actions to execute after collision processing
        /// 3. **Conditional Response**: Only react to certain types of objects
        /// 4. **Forwarded Response**: Delegate to components or other systems
        /// 
        /// Example Implementations:
        /// ```cpp
        /// // Destructible object
        /// void Crate::OnCollision(Actor* other) override {
        ///     if (auto projectile = dynamic_cast&lt;Projectile*&gt;(other)) {
        ///         TakeDamage(projectile->GetDamage());
        ///         if (m_health <= 0) {
        ///             SpawnDebris();
        ///             Destroy();
        ///         }
        ///     }
        /// }
        /// 
        /// // Collectible item
        /// void Coin::OnCollision(Actor* other) override {
        ///     if (auto player = dynamic_cast&lt;Player*&gt;(other)) {
        ///         player->AddScore(m_value);
        ///         PlayCollectSound();
        ///         Destroy();
        ///     }
        /// }
        /// 
        /// // Area trigger
        /// void TriggerZone::OnCollision(Actor* other) override {
        ///     if (auto player = dynamic_cast&lt;Player*&gt;(other)) {
        ///         if (!m_activated) {
        ///             ActivateTrigger();
        ///             m_activated = true;
        ///         }
        ///     }
        /// }
        /// ```
        /// 
        /// Threading Considerations:
        /// - This method may be called from physics simulation threads
        /// - Ensure thread-safe access to shared state if applicable
        /// - Consider deferring complex operations to the main thread
        /// 
        /// Error Handling:
        /// - Validate the other parameter before using it
        /// - Handle unexpected object types gracefully
        /// - Avoid throwing exceptions that might disrupt collision processing
        /// </summary>
        /// <param name="other">
        /// Pointer to the Actor that this object collided with.
        /// The parameter uses a forward declaration to avoid circular dependencies
        /// between ICollidable and Actor headers. This Actor represents the other
        /// participant in the collision and provides access to its properties,
        /// components, and methods for implementing collision response logic.
        /// 
        /// The pointer should be valid during the callback but may become invalid
        /// after the method returns if either object is destroyed during collision processing.
        /// </param>
        virtual void OnCollision(class Actor* other) = 0;
    };
}