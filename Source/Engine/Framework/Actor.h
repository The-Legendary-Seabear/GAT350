#pragma once
#include "Object.h"
#include "Component.h"
#include "Renderer/Texture.h"
#include "Math/Transform.h"
#include <string>
#include <memory>
#include <vector>

namespace neu {
	/// <summary>
	/// Base class for all game objects in the scene.
	/// 
	/// Actor represents any entity in the game world that can be updated, drawn,
	/// and managed by a Scene. It uses a component-based architecture where
	/// functionality is added through attachable Component objects rather than
	/// inheritance, promoting composition over inheritance design principles.
	/// 
	/// Key Features:
	/// - Component-based architecture for flexible functionality
	/// - Transform system for position, rotation, and scale
	/// - Lifecycle management (Start, Update, Draw, Destroyed)
	/// - Serialization support for data-driven content
	/// - Tag system for grouping and querying
	/// - Persistence flags for scene transitions
	/// - Automatic lifespan management
	/// </summary>
	class Actor : public Object {
	public:
		// String identifier for grouping related actors
		// Used by Scene::GetActorsByTag() for flexible queries
		std::string tag;

		// Destruction flag checked by Scene during update loop
		// When true, actor will be removed and Destroyed() will be called
		bool destroyed{ false };

		// Persistence flag for scene transitions
		// If true, actor survives Scene::RemoveAllActors(false)
		// Useful for player objects, UI, or cross-level entities
		bool persistent{ false };

		// Automatic lifetime management in seconds
		// When > 0, decrements each frame and sets destroyed=true at zero
		// Useful for temporary effects, projectiles, particles
		float lifespan{ 0 };

		// Back-reference to containing scene (non-owning pointer)
		// Allows actors to query other actors and access scene-level systems
		// Set by Scene::AddActor(), nullptr before actor is added to scene
		class Scene* scene{ nullptr };

		// Spatial information: position, rotation, scale
		// Used by components for rendering, physics, collision detection
		Transform transform;

	public:
		// Default constructor - initializes with default values
		Actor() = default;

		// Constructor with transform initialization
		// Useful for spawning actors at specific locations
		Actor(const Transform& transform) :
			transform{ transform }
		{
		}

		// Copy constructor - performs deep copy of actor and all components
		// Components are cloned to ensure independent copies
		Actor(const Actor& other);

		// Prototype pattern implementation macro
		// Expands to: virtual std::unique_ptr<Object> Clone() { return std::make_unique<Actor>(*this); }
		// Enables polymorphic cloning of actors for prototype-based instantiation
		// The Clone() method creates a deep copy using the copy constructor
		CLASS_PROTOTYPE(Actor)

		// Override from Object base class
		// Initializes actor and all components after creation
		// Called by Scene after all actors are loaded
		virtual bool Start() override;

		// Override from Object base class
		// Cleanup method called before actor destruction
		// Notifies all components to release resources
		virtual void Destroyed() override;

		// Override from Object base class
		// Deserializes actor data from configuration files
		// Loads properties, transform, and creates components
		void Read(const serial_data_t& value) override;

		// Updates actor logic and all active components
		// Handles lifespan countdown and delegates to components
		// Virtual to allow derived actors to override behavior
		virtual void Update(float dt);

		// Renders actor by drawing all active RendererComponents
		// Delegates to components for actual drawing operations
		// Virtual to allow derived actors to override behavior
		virtual void Draw(class Renderer& renderer);

		/// <summary>
		/// Adds a component to the actor.
		/// 
		/// Transfers ownership of the component to this actor and sets up
		/// the bidirectional owner relationship. Components added through
		/// this method will be updated, drawn, and destroyed with the actor.
		/// </summary>
		/// <param name="component">Unique pointer to component (ownership transferred)</param>
		void AddComponent(std::unique_ptr<Component> component);

		/// <summary>
		/// Checks if the actor has a component of the specified type.
		/// 
		/// Uses dynamic_cast to check for component type, supporting
		/// polymorphic component hierarchies. This is a convenience method
		/// that wraps GetComponent() with a nullptr check.
		/// </summary>
		/// <typeparam name="T">Component-derived type to check for</typeparam>
		/// <returns>True if actor has at least one component of type T</returns>
		template<typename T>
			requires std::derived_from<T, Component>
		bool HasComponent() const;

		/// <summary>
		/// Retrieves the first component of the specified type.
		/// 
		/// Searches through all components and returns the first one that
		/// matches the requested type. Returns nullptr if no matching
		/// component is found. Useful when you know an actor has only
		/// one component of a given type.
		/// </summary>
		/// <typeparam name="T">Component-derived type to retrieve</typeparam>
		/// <returns>Pointer to first matching component, or nullptr</returns>
		template<typename T>
			requires std::derived_from<T, Component>
		T* GetComponent();

		/// <summary>
		/// Retrieves all components of the specified type.
		/// 
		/// Searches through all components and returns a vector containing
		/// all components that match the requested type. Useful when an
		/// actor may have multiple components of the same type (e.g.,
		/// multiple audio sources, multiple colliders).
		/// </summary>
		/// <typeparam name="T">Component-derived type to retrieve</typeparam>
		/// <returns>Vector of pointers to all matching components</returns>
		template<typename T>
			requires std::derived_from<T, Component>
		std::vector<T*> GetComponents();

	protected:
		// Container for all components attached to this actor
		// Uses unique_ptr for automatic memory management
		// Protected to allow derived classes to access components directly
		std::vector<std::unique_ptr<Component>> m_components;
	};

	/// <summary>
	/// Template implementation for checking component existence.
	/// 
	/// Convenience method that delegates to GetComponent() and checks
	/// for nullptr. This provides a cleaner API for existence checks
	/// without requiring the caller to perform nullptr comparison.
	/// </summary>
	template<typename T>
		requires std::derived_from<T, Component>
	inline bool Actor::HasComponent() const
	{
		// Delegate to GetComponent and check if result is valid
		// const_cast needed because GetComponent is non-const
		// (it returns non-const pointer for modification)
		return const_cast<Actor*>(this)->GetComponent<T>() != nullptr;
	}

	/// <summary>
	/// Template implementation for retrieving first component of type.
	/// 
	/// Performs linear search through components using dynamic_cast
	/// for type checking. Returns immediately upon finding first match,
	/// making this efficient when component is near the front of the list.
	/// </summary>
	template<typename T>
		requires std::derived_from<T, Component>
	inline T* Actor::GetComponent()
	{
		// Iterate through all components
		for (auto& component : m_components) {
			// Attempt to cast to requested type
			// dynamic_cast returns nullptr if component is not of type T
			auto result = dynamic_cast<T*>(component.get());

			// If cast succeeded, return immediately (first match)
			if (result) return result;
		}

		// No matching component found
		return nullptr;
	}

	/// <summary>
	/// Template implementation for retrieving all components of type.
	/// 
	/// Performs exhaustive search through all components, collecting
	/// every component that matches the requested type. Unlike GetComponent(),
	/// this continues searching even after finding matches.
	/// </summary>
	template<typename T>
		requires std::derived_from<T, Component>
	inline std::vector<T*> Actor::GetComponents()
	{
		// Container to hold all matching components
		std::vector<T*> results;

		// Iterate through all components
		for (auto& component : m_components) {
			// Attempt to cast to requested type
			auto result = dynamic_cast<T*>(component.get());

			// If cast succeeded, add to results and continue searching
			if (result) {
				results.push_back(result);
			}
		}

		// Return all matching components (may be empty)
		return results;
	}
}