#include "Actor.h"
#include "Renderer/Renderer.h"
#include "Components/RendererComponent.h"

namespace neu {
	// Register Actor class with the Factory system
	// This macro enables creation of Actor instances from string type names
	// Essential for serialization and data-driven content loading
	FACTORY_REGISTER(Actor)

	/// <summary>
	/// Copy constructor - creates a deep copy of an actor.
	/// 
	/// Performs a complete clone of the actor, including all components.
	/// Each component is individually cloned to ensure independence between
	/// the original and copy. This is essential for prototype-based spawning
	/// where multiple actors are created from a single template.
	/// </summary>
	Actor::Actor(const Actor& other) :
	Object{ other },              // Copy base Object properties (name, active)
	tag{ other.tag },             // Copy tag for grouping
	lifespan{ other.lifespan },   // Copy lifespan setting
	transform{ other.transform }  // Copy spatial information
	{
		// Deep copy all components from the source actor
		// Each component must be cloned individually to avoid shared ownership
		for (auto& component : other.m_components) {
			// Clone the component using its virtual Clone() method
			// dynamic_cast ensures we get a Component pointer
			// release() transfers ownership from the returned unique_ptr
			auto clone = std::unique_ptr<Component>(dynamic_cast<Component*>(component->Clone().release()));

			// Add the cloned component to this actor
			// This sets up the owner relationship and adds to m_components
			AddComponent(std::move(clone));
		}
	}

	/// <summary>
	/// Initializes the actor and all its components.
	/// 
	/// Called by Scene after the actor is fully constructed and added to the scene.
	/// This ensures all actors exist before any initialization code runs, allowing
	/// components to safely query the scene for other actors during startup.
	/// </summary>
	bool Actor::Start() {
		// Initialize all components attached to this actor
		// Components may set up references, load resources, or initialize state
		for (auto& component : m_components) {
			component->Start();
		}

		// Return success - could be extended to handle initialization failures
		return true;
	}

	/// <summary>
	/// Cleanup method called before actor destruction.
	/// 
	/// Notifies all components that the actor is being destroyed, giving them
	/// a chance to release resources, unregister from systems, or perform
	/// final cleanup operations before memory is freed.
	/// </summary>
	void Actor::Destroyed() {
		// Notify each component of impending destruction
		// Components should release any resources they're holding
		for (auto& component : m_components) {
			component->Destroyed();
		}
	}

	/// <summary>
	/// Updates the actor's state for the current frame.
	/// 
	/// Handles lifespan countdown and delegates to all active components.
	/// The update process is frame-rate independent through the dt parameter.
	/// Actors marked as destroyed skip updates entirely.
	/// </summary>
	void Actor::Update(float dt)
	{
		// Early exit if actor is already destroyed
		// Prevents updates on actors pending removal
		if (destroyed) return;

		// Automatic lifespan management
		// When lifespan reaches zero, actor marks itself for destruction
		if (lifespan > 0) {
			// Decrement remaining lifetime
			lifespan -= dt;

			// Check if lifespan has expired
			if (lifespan <= 0) {
				// Mark for destruction - Scene will remove on next update
				destroyed = true;
				return; // Skip component updates for destroyed actor
			}
		}

		// Update all active components
		// Each component implements its own Update logic
		for (auto& component : m_components) {
			// Only update components that are currently active
			// Inactive components remain attached but don't process
			if (component->active) component->Update(dt);
		}
	}

	/// <summary>
	/// Renders the actor by drawing all active renderer components.
	/// 
	/// Iterates through components looking for RendererComponent types and
	/// delegates drawing to them. Only active components are rendered, and
	/// destroyed actors skip rendering entirely.
	/// </summary>
	void Actor::Draw(Renderer& renderer)
	{
		// Early exit if actor is destroyed
		// No need to render actors pending removal
		if (destroyed) return;

		// Iterate through all components looking for renderable ones
		for (auto& component : m_components) {
			// Only process active components
			if (component->active) {
				// Attempt to cast to RendererComponent
				// Not all components are renderable (e.g., physics, audio, logic)
				auto rendererComponent = dynamic_cast<RendererComponent*>(component.get());

				// If this component is a renderer, draw it
				if (rendererComponent) {
					rendererComponent->Draw(renderer);
				}
			}
		}
	}

	/// <summary>
	/// Adds a component to the actor.
	/// 
	/// Transfers ownership of the component to this actor and establishes
	/// the bidirectional owner relationship. The component will now be
	/// updated, drawn, and destroyed along with its owner actor.
	/// </summary>
	void Actor::AddComponent(std::unique_ptr<Component> component)
	{
		// Set up back-reference from component to actor
		// Components use this to access actor properties (transform, scene, etc.)
		component->owner = this;

		// Transfer ownership to the actor's component container
		// std::move is required to transfer unique_ptr ownership
		m_components.push_back(std::move(component));
	}

	/// <summary>
	/// Deserializes actor data from configuration files.
	/// 
	/// Loads all actor properties including tag, lifespan, persistence,
	/// transform, and creates/configures all components defined in the
	/// serialized data. Integrates with Factory system for type-safe
	/// component creation.
	/// </summary>
	void Actor::Read(const serial_data_t& value) {
		// Load base Object properties first (name, active state)
		Object::Read(value);

		// Load actor-specific properties
		// SERIAL_READ macro handles safe reading with default values
		SERIAL_READ(value, tag);        // Grouping identifier
		SERIAL_READ(value, lifespan);   // Automatic lifetime (0 = infinite)
		SERIAL_READ(value, persistent); // Scene transition persistence flag

		// Load transform data if present
		// Transform is optional in serialized data
		if (SERIAL_CONTAINS(value, transform))
			transform.Read(SERIAL_AT(value, transform));

		// SECTION: Load and create components
		// Components are defined in a "components" array in the data
		if (SERIAL_CONTAINS(value, components)) {
			// Iterate through each component definition
			for (auto& componentValue : SERIAL_AT(value, components).GetArray()) {

				// Extract the component type string
				// Type is required to know which component class to instantiate
				std::string type;
				if (!SERIAL_READ(componentValue, type)) {
					// Log warning but continue processing other components
					LOG_WARNING("Component missing type in actor '{}'", name);
					continue; // Skip this malformed component
				}

				// Use Factory to create component instance from type string
				// Factory handles type-safe component creation
				auto component = Factory::Instance().Create<Component>(type);
				if (!component) {
					// Component type not registered or creation failed
					LOG_ERROR("Failed to create component of type '{}' for actor '{}'", type, name);
					continue; // Skip this component
				}

				// Load component-specific data from serialized value
				// Each component type implements its own Read() method
				component->Read(componentValue);

				// Add the fully configured component to this actor
				AddComponent(std::move(component));
			}
		}
	}
}