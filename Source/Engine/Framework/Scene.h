#pragma once
#include "Object.h"
#include <string>
#include <vector>
#include <list>
#include <memory>

namespace neu {
    // Forward declaration to avoid circular dependency
    // Scene needs to know about Actor, but full definition not needed in header
    class Actor;

    /// <summary>
    /// Container and manager for all actors within a game level or area.
    /// 
    /// The Scene class serves as the primary container for objects (Actors) and provides
    /// the main game loop functionality (update, draw) for all contained entities. It extends
    /// Object to support serialization, enabling level data to be loaded from configuration files.
    /// 
    /// Key Responsibilities:
    /// - Actor lifecycle management (creation, update, destruction)
    /// - Coordinated updating of all active actors each frame
    /// - Rendering coordination for all visible actors
    /// - Factory integration for loading actors from serialized data
    /// - Prototype registration for efficient actor spawning
    /// - Query interface for finding specific actors or actor types
    /// 
    /// Design Philosophy:
    /// - Centralized management: Single point of control for all actors in a scene
    /// - Data-driven loading: Scenes can be completely defined in external files
    /// - Performance-focused: Efficient iteration and selective processing
    /// - Type-safe queries: Template-based actor retrieval with compile-time type checking
    /// - Memory ownership: Clear ownership model using unique_ptr for automatic cleanup
    /// 
    /// Common Usage Patterns:
    /// 1. **Level Loading**: Load entire scenes from JSON configuration files
    /// 2. **Game Loop Integration**: Update and draw all actors in coordinated fashion
    /// 3. **Actor Management**: Add/remove actors dynamically during gameplay
    /// 4. **Object Queries**: Find specific actors for interaction or game logic
    /// 5. **Prototype Systems**: Register actor templates for efficient spawning
    /// 
    /// Example Scene Structure:
    /// ```json
    /// {
    ///     "name": "Level1",
    ///     "prototypes": [
    ///         {"name": "EnemyTemplate", "type": "Enemy", ...},
    ///         {"name": "PickupTemplate", "type": "Pickup", ...}
    ///     ],
    ///     "actors": [
    ///         {"name": "Player", "type": "Player", "position": {"x": 0, "y": 0}},
    ///         {"name": "Enemy1", "prototype": "EnemyTemplate", ...}
    ///     ]
    /// }
    /// ```
    /// </summary>
    class Scene : public ISerializable {
    public:
        /// <summary>
        /// Constructs a scene with a reference to the parent game instance.
        /// 
        /// The game pointer provides access to global systems and services that
        /// actors within the scene may need, such as input systems, audio managers,
        /// or game-wide state information.
        /// 
        /// The scene maintains a non-owning pointer to the game, as the game typically
        /// owns and manages scene lifecycle. This relationship enables two-way
        /// communication between the scene and the broader game context.
        /// </summary>
        /// <param name="game">Pointer to the parent Game instance that owns this scene</param>
        Scene() = default;

        // Override from Object base class
        // Initializes all actors after scene is fully loaded
        bool Start();

        // Override from Object base class
        // Cleans up all actors before scene destruction
        void Destroyed();

        /// <summary>
        /// Loads a complete scene from a named configuration file.
        /// 
        /// This method provides the primary entry point for data-driven scene creation.
        /// It loads serialized scene data, creates all actors and prototypes, and
        /// prepares the scene for immediate use.
        /// 
        /// Loading process:
        /// 1. Load and parse the serialized scene data
        /// 2. Process prototypes and register them with the Factory
        /// 3. Create all actors defined in the scene
        /// 4. Initialize all actors by calling their Start() methods
        /// 
        /// The method handles both prototypes (reusable actor templates) and direct
        /// actor definitions, enabling flexible scene composition strategies.
        /// </summary>
        /// <param name="sceneName">Name/path of the scene file to load</param>
        /// <returns>True if the scene loaded successfully, false on any error</returns>
        bool Load(const std::string& sceneName);

        /// <summary>
        /// Deserializes scene data from serialized format.
        /// 
        /// This method implements the ISerializable interface to support loading
        /// scene configuration from external data sources. It processes both
        /// prototypes and actor definitions to populate the scene.
        /// 
        /// The implementation supports:
        /// - Base Object properties (name, active state)
        /// - Prototype definitions for reusable actor templates
        /// - Direct actor definitions for immediate scene population
        /// - Factory integration for type-safe actor creation
        /// </summary>
        /// <param name="value">Serialized data containing scene configuration</param>
        void Read(const serial_data_t& value) override;

        /// <summary>
        /// Updates all active actors in the scene for the current frame.
        /// 
        /// This method implements the main update loop for the scene, calling
        /// Update() on all active actors and handling cleanup of destroyed actors.
        /// Only actors marked as active participate in the update cycle.
        /// 
        /// Update process:
        /// 1. Iterate through all actors in the scene
        /// 2. Update each active actor with the frame's delta time
        /// 3. Remove any actors marked as destroyed after updating
        /// 
        /// Performance considerations:
        /// - Inactive actors are skipped for efficiency
        /// - Destroyed actors are removed in a single pass after all updates
        /// - Update order is based on actor addition order (not guaranteed to be stable)
        /// </summary>
        /// <param name="dt">Time elapsed since the last frame update, in seconds</param>
        void Update(float dt);

        /// <summary>
        /// Renders all active actors in the scene using the provided renderer.
        /// 
        /// This method coordinates the rendering of all visible actors, providing
        /// them with access to the renderer for drawing operations. Only active
        /// actors participate in the rendering process.
        /// 
        /// Rendering process:
        /// 1. Iterate through all actors in the scene
        /// 2. Call Draw() on each active actor with the renderer
        /// 3. Actors handle their own rendering logic and resource management
        /// 
        /// The scene does not perform any rendering itself - it serves as a
        /// coordinator that ensures all actors have the opportunity to render.
        /// </summary>
        /// <param name="renderer">Renderer instance used for all drawing operations</param>
        void Draw(class Renderer& renderer);

        /// <summary>
        /// Adds an actor to the scene with optional immediate initialization.
        /// 
        /// This method transfers ownership of the actor to the scene and optionally
        /// initializes it immediately. The actor's scene pointer is set to enable
        /// bidirectional communication between the actor and its containing scene.
        /// 
        /// Ownership transfer:
        /// - The scene takes full ownership of the actor via std::move
        /// - The actor's lifetime is now managed by the scene
        /// - The actor will be automatically destroyed when the scene is destroyed
        /// 
        /// Initialization control:
        /// - If start=true, the actor's Start() method is called immediately
        /// - If start=false, Start() must be called manually later (useful for batch operations)
        /// </summary>
        /// <param name="actor">Unique pointer to the actor to add (ownership transferred)</param>
        /// <param name="start">Whether to immediately call Start() on the actor (default: true)</param>
        void AddActor(std::unique_ptr<Actor> actor, bool start = true);

        /// <summary>
        /// Removes actors from the scene based on their persistence settings.
        /// 
        /// This method provides selective or complete actor removal, respecting
        /// the persistent flag that some actors may use to survive scene transitions.
        /// 
        /// Removal behavior:
        /// - If force=false: Only non-persistent actors are removed
        /// - If force=true: All actors are removed regardless of persistence
        /// 
        /// Use cases:
        /// - Scene transitions where some actors should carry over
        /// - Level resets where everything should be cleared
        /// - Cleanup operations during game state changes
        /// </summary>
        /// <param name="force">If true, removes all actors; if false, only removes non-persistent actors</param>
        void RemoveAllActors(bool force = false);

        /// <summary>
        /// Retrieves all actors in the scene of a specific type.
        /// 
        /// This template method performs type-safe retrieval of actors, using
        /// dynamic_cast to ensure type correctness. It returns raw pointers for
        /// efficiency while the scene retains ownership.
        /// 
        /// Type safety:
        /// - Template parameter must derive from Actor (enforced by C++20 concepts)
        /// - dynamic_cast ensures safe type conversion
        /// - Invalid casts result in nullptr (excluded from results)
        /// 
        /// Performance considerations:
        /// - O(n) operation that iterates through all actors
        /// - dynamic_cast has some overhead but provides safety
        /// - Results are returned by value (vector copy)
        /// 
        /// Usage examples:
        /// ```cpp
        /// auto enemies = scene->GetActorsOfType&lt;Enemy&gt;();
        /// auto powerups = scene->GetActorsOfType&lt;PowerUp&gt;();
        /// ```
        /// </summary>
        /// <typeparam name="T">Actor-derived type to search for (defaults to Actor)</typeparam>
        /// <returns>Vector of pointers to actors of the specified type</returns>
        template<typename T = Actor>
            requires std::derived_from<T, Actor>
        std::vector<T*> GetActorsOfType();

        /// <summary>
        /// Retrieves the first actor with the specified name, cast to the given type.
        /// 
        /// This method performs case-insensitive name matching to find a specific
        /// actor and attempts to cast it to the requested type. Returns the first
        /// matching actor found, or nullptr if no match exists or the cast fails.
        /// 
        /// Search behavior:
        /// - Case-insensitive string comparison using equalsIgnoreCase()
        /// - Returns first match found (search order is actor addition order)
        /// - Type safety ensured through dynamic_cast
        /// 
        /// Use cases:
        /// - Finding specific named actors (player, boss, key objects)
        /// - Accessing unique actors by their identifiers
        /// - Type-safe retrieval when actor type is known
        /// 
        /// Example usage:
        /// ```cpp
        /// auto player = scene->GetActorByName&lt;Player&gt;("MainPlayer");
        /// auto door = scene->GetActorByName&lt;Door&gt;("ExitDoor");
        /// ```
        /// </summary>
        /// <typeparam name="T">Actor-derived type to cast the result to (defaults to Actor)</typeparam>
        /// <param name="name">Name of the actor to search for (case-insensitive)</param>
        /// <returns>Pointer to the first matching actor of type T, or nullptr if not found</returns>
        template<typename T = Actor>
            requires std::derived_from<T, Actor>
        T* GetActorByName(const std::string& name);

        /// <summary>
        /// Retrieves all actors with the specified tag, cast to the given type.
        /// 
        /// This method finds all actors that have a matching tag value and are
        /// of the specified type. Tags provide a flexible grouping mechanism
        /// for actors that share common properties or behaviors.
        /// 
        /// Tag system benefits:
        /// - Flexible grouping independent of type hierarchy
        /// - Multiple actors can share the same tag
        /// - Case-insensitive tag matching for user convenience
        /// - Efficient way to find functionally related actors
        /// 
        /// Common tag usage:
        /// - "enemy": All hostile actors
        /// - "collectible": Items that can be picked up
        /// - "destructible": Objects that can be destroyed
        /// - "interactive": Objects the player can interact with
        /// 
        /// Example usage:
        /// ```cpp
        /// auto enemies = scene->GetActorsByTag&lt;Enemy&gt;("hostile");
        /// auto pickups = scene->GetActorsByTag&lt;Item&gt;("collectible");
        /// ```
        /// </summary>
        /// <typeparam name="T">Actor-derived type to filter and cast results to (defaults to Actor)</typeparam>
        /// <param name="tag">Tag value to match against actor tags (case-insensitive)</param>
        /// <returns>Vector of pointers to actors of type T with the specified tag</returns>
        template<typename T = Actor>
            requires std::derived_from<T, Actor>
        std::vector<T*> GetActorsByTag(const std::string& tag);

    private:

        /// <summary>
        /// Container for all actors in the scene.
        /// 
        /// Uses std::list for efficient insertion and removal of actors during
        /// gameplay. The list stores unique_ptr for automatic memory management
        /// and clear ownership semantics.
        /// 
        /// Container choice rationale:
        /// - std::list: Efficient insertion/removal without iterator invalidation
        /// - unique_ptr: Clear ownership and automatic cleanup
        /// - Stable iteration: Actors can be safely added/removed during iteration
        /// 
        /// Performance characteristics:
        /// - Actor iteration: O(n) for update/draw operations
        /// - Actor addition: O(1) insertion at end
        /// - Actor removal: O(n) for search, O(1) for removal once found
        /// </summary>
        std::list<std::unique_ptr<Actor>> m_actors;
    };

    // ============================================================================
    // TEMPLATE IMPLEMENTATIONS
    // These must be in the header because templates are instantiated at compile time
    // ============================================================================

    /// <summary>
    /// Template implementation for retrieving actors by type.
    /// 
    /// Iterates through all actors and uses dynamic_cast to safely identify
    /// and collect actors of the specified type. This provides type-safe
    /// actor retrieval with runtime type checking.
    /// </summary>
    template<typename T>
        requires std::derived_from<T, Actor>
    inline std::vector<T*> Scene::GetActorsOfType()
    {
        // Container to hold matching actors
        std::vector<T*> results;

        // Iterate through all actors in the scene
        for (auto& actor : m_actors) {
            // Attempt to cast the actor to the requested type
            // dynamic_cast returns nullptr if the cast fails (wrong type)
            T* object = dynamic_cast<T*>(actor.get());

            // Check if the cast was successful
            if (object) {
                // Cast succeeded - this actor is of type T
                // Add the raw pointer to results (scene retains ownership)
                results.push_back(object);
            }
        }

        // Return vector of all matching actors
        return results;
    }

    /// <summary>
    /// Retrieves an actor from the scene by name and casts it to the specified type.
    /// 
    /// Performs case-insensitive name matching and type-safe casting to find
    /// a specific named actor. Returns the first match found, enabling unique
    /// actor identification by name.
    /// </summary>
    /// <typeparam name="T">The type to which the found actor should be cast.</typeparam>
    /// <param name="name">The name of the actor to search for (case-insensitive).</param>
    /// <returns>A pointer to the actor cast to type T if found and the cast is successful; otherwise, nullptr.</returns>
    template<typename T>
        requires std::derived_from<T, Actor>
    inline T* Scene::GetActorByName(const std::string& name)
    {
        // Iterate through all actors looking for name match
        for (auto& actor : m_actors) {
            // Perform case-insensitive name comparison
            // equalsIgnoreCase is a utility function for string comparison
            if (neu::equalsIgnoreCase(actor->name, name)) {
                // Name matches - attempt to cast to the requested type
                T* object = dynamic_cast<T*>(actor.get());

                // Check if the cast was successful
                if (object) {
                    // Both name and type match - return this actor
                    return object;
                }
                // If cast failed, continue searching in case there's another
                // actor with the same name but different type
            }
        }

        // No matching actor found with correct name and type
        return nullptr;
    }

    /// <summary>
    /// Retrieves all actors in the scene with a tag matching the specified value and of the specified type.
    /// 
    /// Combines tag-based filtering with type-based filtering to provide flexible
    /// actor queries. Useful for finding groups of related actors that share
    /// both a common tag and type.
    /// </summary>
    /// <typeparam name="T">The type to which actors should be cast. Only actors of this type are included in the results.</typeparam>
    /// <param name="tag">The tag to match against each actor's tag (case-insensitive).</param>
    /// <returns>A vector of pointers to actors of type T whose tag matches the specified tag.</returns>
    template<typename T>
        requires std::derived_from<T, Actor>
    inline std::vector<T*> Scene::GetActorsByTag(const std::string& tag)
    {
        // Container to hold matching actors
        std::vector<T*> results;

        // Iterate through all actors in the scene
        for (auto& actor : m_actors) {
            // Check if actor's tag matches the requested tag (case-insensitive)
            // equalsIgnoreCase provides user-friendly tag matching
            if (neu::equalsIgnoreCase(actor->tag, tag)) {
                // Tag matches - attempt to cast to the requested type
                T* object = dynamic_cast<T*>(actor.get());

                // Check if the cast was successful
                if (object) {
                    // Both tag and type match - add to results
                    results.push_back(object);
                }
                // If cast fails, skip this actor (wrong type)
            }
        }

        // Return vector of all actors with matching tag and type
        return results;
    }
}