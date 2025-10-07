#include "Scene.h"
#include "Renderer/Renderer.h"

namespace neu {
    /// <summary>
    /// Updates all actors in the scene by advancing their state based on the elapsed time.
    /// 
    /// This method implements the main update loop for the scene, processing all active
    /// actors and performing cleanup of destroyed actors. The update process respects
    /// the active state of actors to allow for selective processing.
    /// 
    /// Update algorithm:
    /// 1. Iterate through all actors in the scene
    /// 2. Update only those actors marked as active
    /// 3. Remove actors marked as destroyed after all updates complete
    /// 
    /// The two-phase approach (update then cleanup) ensures that:
    /// - All active actors get a chance to update before any are removed
    /// - Actors can mark themselves or others for destruction during update
    /// - Container modification happens after iteration to avoid iterator invalidation
    /// 
    /// Performance optimizations:
    /// - Inactive actors are skipped entirely
    /// - Destruction cleanup uses efficient std::erase_if
    /// - Single-pass destruction removes all destroyed actors at once
    /// </summary>
    /// <param name="dt">The time elapsed since the last update, in seconds.</param>
    void Scene::Update(float dt) {
        // PHASE 1: Update all active actors
        // Loop through every actor in the scene container
        for (auto& actor : m_actors) {
            // Check the active flag before processing
            // This allows actors to be temporarily disabled without removal
            if (actor->active) {
                // Delegate to the actor's own Update implementation
                // dt allows for frame-rate independent movement and timing
                actor->Update(dt);
            }
        }

        // PHASE 2: Cleanup destroyed actors
        // Call Destroyed() on actors before removing them to allow cleanup
        // Then use std::erase_if for efficient removal (C++20 feature)
        std::erase_if(m_actors, [](auto& actor) {
            // Check if this actor should be removed
            if (actor->destroyed) {
                // Call Destroyed() to give the actor a chance to clean up
                // (release resources, notify other systems, etc.)
                actor->Destroyed();
                // Return true to remove this actor from the container
                return true;
            }
            // Actor is not destroyed, keep it in the scene
            return false;
            });
    }

    /// <summary>
    /// Draws all actors in the scene using the specified renderer.
    /// 
    /// This method coordinates the rendering phase for all visible actors in the scene.
    /// Only active actors participate in rendering, allowing for dynamic visibility
    /// control without removing actors from the scene.
    /// 
    /// Rendering process:
    /// 1. Iterate through all actors in the scene
    /// 2. Call Draw() on each active actor
    /// 3. Actors handle their own rendering logic and resource management
    /// 
    /// The scene serves as a rendering coordinator, ensuring all actors have
    /// access to the renderer while not imposing any specific rendering order
    /// or techniques. Individual actors are responsible for their own drawing logic.
    /// 
    /// Performance considerations:
    /// - Inactive actors are skipped for rendering efficiency
    /// - No depth sorting or rendering optimization is performed at scene level
    /// - Actors may implement their own culling or LOD systems
    /// </summary>
    /// <param name="renderer">The renderer used to draw the actors.</param>
    void Scene::Draw(Renderer& renderer) {
        // Iterate through all actors in the scene
        for (auto& actor : m_actors) {
            // Only render actors that are marked as active
            // This parallels the Update() logic for consistency
            if (actor->active) {
                // Pass the renderer to each actor
                // Each actor is responsible for its own drawing implementation
                actor->Draw(renderer);
            }
        }
    }

    /// <summary>
    /// Adds an actor to the scene by transferring ownership of the actor.
    /// 
    /// This method incorporates a new actor into the scene, setting up the
    /// bidirectional relationship between the actor and scene, and optionally
    /// initializing the actor immediately.
    /// 
    /// Integration process:
    /// 1. Set the actor's scene pointer to establish parent relationship
    /// 2. Optionally call Start() to initialize the actor
    /// 3. Transfer ownership to the scene's actor container
    /// 
    /// The scene pointer enables actors to access scene-wide functionality
    /// and query other actors. The start parameter allows for batch actor
    /// addition without immediate initialization (useful during scene loading).
    /// </summary>
    /// <param name="actor">A unique pointer to the actor to be added. Ownership of the actor is transferred to the scene.</param>
    /// <param name="start">Whether to immediately call Start() on the actor for initialization</param>
    void Scene::AddActor(std::unique_ptr<Actor> actor, bool start) {
        // Validate that we're not trying to add a null pointer
        // ASSERT_MSG will help catch bugs during development
        ASSERT_MSG(actor, "Attempted to add null actor to scene");

        // Establish back-reference from actor to scene
        // This allows actors to query the scene, find other actors, etc.
        actor->scene = this;

        // Optionally initialize the actor immediately
        // During batch loading, we skip Start() and call it later for all actors
        if (start) actor->Start();

        // Transfer ownership to the scene's container
        // std::move is required to transfer unique_ptr ownership
        // push_back adds to the end of the list
        m_actors.push_back(std::move(actor));
    }

    /// <summary>
    /// Removes actors from the scene based on persistence flags.
    /// 
    /// This method provides selective actor removal, allowing certain actors
    /// to persist across scene changes while removing others. This is useful
    /// for maintaining continuity of important game objects across level transitions.
    /// 
    /// Removal logic:
    /// - Non-persistent actors are always removed
    /// - Persistent actors are only removed if force=true
    /// - Uses iterator-based removal for safe container modification
    /// 
    /// Common use cases:
    /// - Scene transitions where some objects should carry over
    /// - Level resets requiring complete cleanup
    /// - Gameplay events that clear specific actor categories
    /// </summary>
    /// <param name="force">If true, removes all actors regardless of persistence; if false, preserves persistent actors</param>
    void Scene::RemoveAllActors(bool force) {
        // Use manual iterator loop for conditional removal
        // std::erase_if can't be used here due to complex removal logic
        for (auto iter = m_actors.begin(); iter != m_actors.end(); ) {
            // Determine if this actor should be removed
            // Remove if: not persistent OR force removal is requested
            if (!(*iter)->persistent || force) {
                // Call Destroyed() on the actor before removing it
                // This allows the actor to clean up resources, save state, etc.
                (*iter)->Destroyed();

                // erase() invalidates current iterator but returns next valid iterator
                // This allows us to continue iteration safely
                iter = m_actors.erase(iter);
            }
            else {
                // This actor survives - manually advance to next
                // Don't use iter++ in the for loop due to conditional advancement
                iter++;
            }
        }
    }

    bool Scene::Start() {
        // Initialize all actors after the scene is fully constructed
        // This ensures all actors exist before any Start() methods run
        // allowing actors to safely find and reference other actors
        for (auto& actor : m_actors) {
            // Call each actor's initialization routine
            actor->Start();
        }

        // Return success - could be extended to handle initialization failures
        return true;
    }

    void Scene::Destroyed() {
        // Notify all actors that the scene is being destroyed
        // Gives actors a chance to clean up resources, save state, etc.
        for (auto& actor : m_actors) {
            actor->Destroyed();
        }

        // Clear the actor container
        // unique_ptr ensures all actors are properly deleted
        m_actors.clear();
    }

    /// <summary>
    /// Loads a complete scene from a named configuration file.
    /// 
    /// This method provides the primary entry point for data-driven scene creation,
    /// loading serialized scene data and preparing it for immediate gameplay use.
    /// 
    /// Loading process:
    /// 1. Load and parse the serialized scene document
    /// 2. Process scene data through Read() method (prototypes and actors)
    /// 3. Initialize all loaded actors by calling their Start() methods
    /// 4. Return success/failure status for error handling
    /// 
    /// The method handles complete scene setup, including prototype registration
    /// and actor initialization, making the scene immediately ready for use.
    /// </summary>
    /// <param name="sceneName">Name/path of the scene file to load</param>
    /// <returns>True if the scene loaded successfully, false on any error</returns>
    bool Scene::Load(const std::string& sceneName) {
        // Create a document to hold the parsed serialized data
        neu::serial::document_t document;

        // Attempt to load and parse the scene file
        // Load() handles file I/O and JSON/serialization parsing
        if (!neu::serial::Load(sceneName, document)) {
            // Log error with scene name for debugging
            LOG_ERROR("Could not load scene {}", sceneName);
            return false; // Early return on failure
        }

        // Process the loaded document to populate the scene
        // Read() handles prototypes and actors sections
        Read(document);

        // Scene loaded successfully
        return true;
    }

    /// <summary>
    /// Deserializes scene data from serialized format.
    /// 
    /// This method processes serialized scene configuration to populate the scene
    /// with prototypes and actors. It handles two main sections of scene data:
    /// prototypes (reusable actor templates) and actors (immediate scene content).
    /// 
    /// Processing order:
    /// 1. Load base Object properties (name, active state)
    /// 2. Process prototypes and register them with the Factory
    /// 3. Process actors and add them to the scene
    /// 
    /// The method integrates with the Factory system to enable both direct actor
    /// creation and prototype-based instantiation, providing flexible scene
    /// composition strategies.
    /// </summary>
    /// <param name="value">Serialized data containing scene configuration</param>
    void Scene::Read(const serial_data_t& value) {
        // Load base Object properties first (name, active, etc.)
        // This calls the parent class's Read() implementation
        Object::Read(value);

        // SECTION 1: Process prototype definitions
        // Check if the serialized data contains a "prototypes" section
        if (SERIAL_CONTAINS(value, prototypes)) {
            // Iterate through each prototype definition in the array
            for (auto& actorValue : SERIAL_AT(value, prototypes).GetArray()) {
                // Create a new base Actor instance via Factory
                // This uses the Factory pattern for type-safe object creation
                auto actor = Factory::Instance().Create<Actor>("Actor");

                // Load the actor's configuration from serialized data
                // This populates all actor properties (transform, components, etc.)
                actor->Read(actorValue);

                // Extract the actor's name to use as the prototype identifier
                std::string name = actor->name;

                // Register this configured actor as a reusable prototype
                // Other actors can now be instantiated from this template
                Factory::Instance().RegisterPrototype<Actor>(name, std::move(actor));
            }
        }

        // SECTION 2: Process direct actor definitions
        // Check if the serialized data contains an "actors" section
        if (SERIAL_CONTAINS(value, actors)) {
            // Iterate through each actor definition in the array
            for (auto& actorValue : SERIAL_AT(value, actors).GetArray()) {
                // Create a new Actor instance via Factory
                // Actors may reference prototypes defined above
                auto actor = Factory::Instance().Create<Actor>("Actor");

                // Load the actor's configuration from serialized data
                actor->Read(actorValue);

                // Add the actor to the scene without starting it yet
                // start=false defers initialization until all actors are loaded
                // This ensures all actors exist before any Start() methods run
                AddActor(std::move(actor), false);
            }
        }
    }
}