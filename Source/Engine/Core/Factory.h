#pragma once
#include "Framework/Actor.h"
#include "StringHelper.h"
#include "Singleton.h"
#include "Logger.h"

#include <map>
#include <memory>
#include <string>

/// <summary>
/// Convenience macro for automatic factory registration of a class.
/// This macro creates a static registration object that automatically registers
/// the specified class with the Factory when the program starts.
/// 
/// The macro generates:
/// 1. A unique registration class for the specified type
/// 2. A constructor that calls Factory::Register() with the class name
/// 3. A static instance that triggers registration at program startup
/// 
/// Usage: Place FACTORY_REGISTER(MyClass) after the class definition
/// Example:
///   class MyActor : public Actor { ... };
///   FACTORY_REGISTER(MyActor)
/// 
/// This allows creating instances via: Factory::Instance().Create&lt;MyActor&gt;("MyActor")
/// </summary>
/// <param name="classname">The class name to register (without quotes)</param>
#define FACTORY_REGISTER(classname) \
class Register##classname {         \
public:                             \
	Register##classname() {         \
		neu::Factory::Instance().Register<classname>(#classname); \
	}                               \
};                                  \
Register##classname register_instance;

namespace neu {
	/// <summary>
	/// Abstract base class for object creators in the Factory pattern.
	/// This class defines the interface that all concrete creators must implement.
	/// Creators are responsible for instantiating objects of specific types
	/// while maintaining type erasure at the factory level.
	/// </summary>
	class CreatorBase {
	public:
		/// <summary>
		/// Virtual destructor to ensure proper cleanup of derived creator types.
		/// Essential for polymorphic destruction when creators are stored as base pointers.
		/// </summary>
		virtual ~CreatorBase() = default;

		/// <summary>
		/// Pure virtual method for creating object instances.
		/// Derived classes must implement this to create objects of their specific type.
		/// </summary>
		/// <returns>A unique pointer to a newly created Object instance</returns>
		virtual std::unique_ptr<Object> Create() = 0;
	};

	/// <summary>
	/// Template creator class that instantiates objects using their default constructor.
	/// This creator type is used for classes that can be created with new T().
	/// The template parameter T must derive from Object to ensure type safety.
	/// </summary>
	/// <typeparam name="T">The specific object type to create, must inherit from Object</typeparam>
	template <typename T>
		requires std::derived_from<T, Object>
	class Creator : public CreatorBase {
	public:
		/// <summary>
		/// Creates a new instance of type T using its default constructor.
		/// The object is created on the heap and wrapped in a unique_ptr for automatic memory management.
		/// </summary>
		/// <returns>A unique pointer to the newly created T instance, cast to Object base type</returns>
		std::unique_ptr<Object> Create() override {
			return std::make_unique<T>();
		}
	};

	/// <summary>
	/// Template creator class that creates objects by cloning a prototype instance.
	/// This implements the Prototype pattern, allowing creation of new objects
	/// by copying an existing configured instance rather than using default construction.
	/// 
	/// Useful for:
	/// - Objects with complex initialization
	/// - Pre-configured object templates
	/// - Objects that need specific state on creation
	/// </summary>
	/// <typeparam name="T">The specific object type to clone, must inherit from Object</typeparam>
	template <typename T>
		requires std::derived_from<T, Object>
	class PrototypeCreator : public CreatorBase {
	public:
		/// <summary>
		/// Constructor that takes ownership of a prototype instance.
		/// The prototype will be used as a template for all future Create() calls.
		/// </summary>
		/// <param name="prototype">The prototype instance to use for cloning (ownership transferred)</param>
		PrototypeCreator(std::unique_ptr<T> prototype) :
			m_prototype{ std::move(prototype) }
		{
		}

		/// <summary>
		/// Creates a new instance by cloning the stored prototype.
		/// This calls the prototype's Clone() method, which must be implemented by the target class.
		/// </summary>
		/// <returns>A unique pointer to the newly cloned Object instance</returns>
		std::unique_ptr<Object> Create() override {
			return m_prototype->Clone();
		}

	private:
		/// <summary>
		/// The prototype instance used as a template for cloning.
		/// This object is never modified after construction and serves as the blueprint
		/// for all objects created by this creator.
		/// </summary>
		std::unique_ptr<T> m_prototype;
	};

	/// <summary>
	/// Factory class implementing the Factory pattern for object creation.
	/// This singleton provides centralized object creation with runtime type resolution.
	/// 
	/// Key features:
	/// - Case-insensitive name lookup (names converted to lowercase)
	/// - Type safety through C++20 concepts
	/// - Support for both constructor-based and prototype-based creation
	/// - Automatic memory management with unique_ptr
	/// - Runtime type checking with dynamic_cast
	/// 
	/// The factory maintains a registry of creators mapped to string names,
	/// allowing objects to be created by name without compile-time knowledge of the type.
	/// </summary>
	class Factory : public Singleton<Factory> {
	public:
		/// <summary>
		/// Registers a class for creation using its default constructor.
		/// Creates a Creator&lt;T&gt; instance and associates it with the given name.
		/// The name is converted to lowercase for case-insensitive lookup.
		/// </summary>
		/// <typeparam name="T">The class type to register, must inherit from Object</typeparam>
		/// <param name="name">The string identifier for this class type</param>
		template<typename T>
			requires std::derived_from<T, Object>
		void Register(const std::string& name);

		/// <summary>
		/// Registers a class for creation using prototype-based cloning.
		/// Creates a PrototypeCreator&lt;T&gt; instance with the provided prototype.
		/// All future Create() calls for this name will clone the prototype.
		/// </summary>
		/// <typeparam name="T">The class type to register, must inherit from Object</typeparam>
		/// <param name="name">The string identifier for this prototype</param>
		/// <param name="prototype">The prototype instance to use for cloning (ownership transferred)</param>
		template<typename T>
			requires std::derived_from<T, Object>
		void RegisterPrototype(const std::string& name, std::unique_ptr<T> prototype);

		/// <summary>
		/// Creates an instance of the class registered under the given name.
		/// Performs case-insensitive lookup and attempts to cast the result to type T.
		/// 
		/// Process:
		/// 1. Convert name to lowercase for lookup
		/// 2. Find creator in registry
		/// 3. Call creator's Create() method
		/// 4. Dynamic cast result to requested type T
		/// 5. Return typed unique_ptr or nullptr on failure
		/// </summary>
		/// <typeparam name="T">The expected type of the created object, defaults to Object</typeparam>
		/// <param name="name">The string identifier of the class to create</param>
		/// <returns>A unique pointer to the created object of type T, or nullptr if creation failed</returns>
		template<typename T = Object>
			requires std::derived_from<T, Object>
		std::unique_ptr<T> Create(const std::string& name);

		/// <summary>
		/// Removes all registered creators from the factory.
		/// This clears the entire registry, effectively unregistering all classes.
		/// Useful for cleanup or resetting the factory state.
		/// </summary>
		void RemoveAll() { m_registry.clear(); }

	private:
		/// <summary>
		/// Internal registry mapping string names to creator instances.
		/// Key: Lowercase class name for case-insensitive lookup
		/// Value: Unique pointer to creator that can instantiate the associated class
		/// 
		/// The map stores creators as base CreatorBase pointers, providing type erasure
		/// while maintaining the ability to create objects of any registered type.
		/// </summary>
		std::map<std::string, std::unique_ptr<CreatorBase>> m_registry;
	};

	/// <summary>
	/// Template implementation for class registration.
	/// Creates a Creator instance for the specified type and adds it to the registry.
	/// </summary>
	template<typename T>
		requires std::derived_from<T, Object>
	inline void Factory::Register(const std::string& name) {
		// Convert name to lowercase for case-insensitive storage and lookup
		std::string key = toLower(name);

		// Create and store a new Creator instance for type T
		// The Creator will use T's default constructor for object creation
		m_registry[key] = std::make_unique<Creator<T>>();

		// Log successful registration for debugging
		LOG_INFO("{} added to factory.", name);
	}

	/// <summary>
	/// Template implementation for prototype registration.
	/// Creates a PrototypeCreator instance with the provided prototype.
	/// </summary>
	template<typename T>
		requires std::derived_from<T, Object>
	inline void Factory::RegisterPrototype(const std::string& name, std::unique_ptr<T> prototype)
	{
		// Convert name to lowercase for case-insensitive storage and lookup
		std::string key = toLower(name);

		// Create and store a PrototypeCreator with the provided prototype
		// Future Create() calls will clone this prototype instance
		m_registry[key] = std::make_unique<PrototypeCreator<T>>(std::move(prototype));

		// Log successful prototype registration
		LOG_INFO("{} prototype added to factory.", name);
	}

	/// <summary>
	/// Template implementation for object creation.
	/// Handles lookup, creation, and type casting with comprehensive error checking.
	/// </summary>
	template<typename T>
		requires std::derived_from<T, Object>
	inline std::unique_ptr<T> Factory::Create(const std::string& name) {
		// Convert name to lowercase for case-insensitive lookup
		std::string key = toLower(name);

		// Search for creator in registry
		auto it = m_registry.find(key);
		if (it != m_registry.end()) {
			// Creator found - attempt object creation
			auto object = it->second->Create();

			// Attempt to cast to requested type T
			T* derived = dynamic_cast<T*>(object.get());
			if (derived) {
				// Cast successful - transfer ownership to typed unique_ptr
				object.release();  // Release from base unique_ptr
				return std::unique_ptr<T>(derived);  // Create typed unique_ptr
			}

			// Cast failed - type mismatch between registered and requested type
			LOG_ERROR("Type mismatch of factory object: {}", name);
		}
		else {
			// Creator not found in registry
			LOG_ERROR("Could not create factory object: {}", name);
		}

		// Return nullptr on any failure (not found or type mismatch)
		return nullptr;
	}

	/// <summary>
	/// Convenience function for creating Actor instances with default transform.
	/// This is a wrapper around Factory::Create() specifically for Actor-derived types.
	/// The created Actor will have default transform values (position=0,0, rotation=0, scale=1).
	/// </summary>
	/// <typeparam name="T">The Actor-derived type to create, defaults to Actor</typeparam>
	/// <param name="name">The string identifier of the Actor class to instantiate</param>
	/// <returns>A unique pointer to the created Actor instance, or nullptr if creation failed</returns>
	template<typename T = Actor>
		requires std::derived_from<T, Actor>
	std::unique_ptr<T> Instantiate(const std::string& name) {
		return Factory::Instance().Create<T>(name);
	}

	/// <summary>
	/// Convenience function for creating Actor instances with specified transform parameters.
	/// Creates an Actor and immediately sets its transform with the provided values.
	/// </summary>
	/// <typeparam name="T">The Actor-derived type to create, defaults to Actor</typeparam>
	/// <param name="name">The string identifier of the Actor class to instantiate</param>
	/// <param name="position">The initial position (x, y coordinates)</param>
	/// <param name="rotation">The initial rotation angle</param>
	/// <param name="scale">The initial scale factor</param>
	/// <returns>A unique pointer to the created Actor with configured transform</returns>
	template<typename T = Actor>
		requires std::derived_from<T, Actor>
	std::unique_ptr<T> Instantiate(const std::string& name, const vec2& position, float rotation, float scale) {
		// Create the Actor instance
		auto instance = Factory::Instance().Create<T>(name);
		// Configure transform with provided parameters
		instance->transform = Transform{ position, rotation, scale };
		return instance;
	}

	/// <summary>
	/// Convenience function for creating Actor instances with a complete Transform object.
	/// Creates an Actor and copies the provided Transform to its transform member.
	/// </summary>
	/// <typeparam name="T">The Actor-derived type to create, defaults to Actor</typeparam>
	/// <param name="name">The string identifier of the Actor class to instantiate</param>
	/// <param name="transform">The Transform object to copy to the new Actor</param>
	/// <returns>A unique pointer to the created Actor with the specified transform</returns>
	template<typename T = Actor>
		requires std::derived_from<T, Actor>
	std::unique_ptr<T> Instantiate(const std::string& name, const Transform& transform) {
		// Create the Actor instance
		auto instance = Factory::Instance().Create<T>(name);
		// Copy the provided transform
		instance->transform = transform;
		return instance;
	}
}