#pragma once
#include "Core/StringHelper.h"
#include "Core/Singleton.h"
#include "Core/Logger.h"
#include "Resource.h"
#include <string>
#include <map>
#include <iostream>

namespace neu {
    /// <summary>
    /// Centralized resource management system that handles loading, caching, and lifetime management
    /// of game resources such as textures, sounds, models, and other assets.
    /// 
    /// Key features:
    /// - Automatic caching: Resources are loaded once and reused
    /// - Type safety: Template-based with compile-time type checking
    /// - Memory management: Uses shared_ptr for automatic cleanup
    /// - Case-insensitive keys: Resource IDs are normalized to lowercase
    /// - Flexible loading: Supports forwarding arbitrary arguments to resource constructors
    /// 
    /// The ResourceManager uses the Singleton pattern to provide global access throughout the engine.
    /// </summary>
    class ResourceManager : public Singleton<ResourceManager> {
    public:
        /// <summary>
        /// Removes all cached resources from the manager.
        /// This clears the internal resource cache, which may trigger resource destruction
        /// if no other references exist. Useful for memory cleanup or level transitions.
        /// </summary>
        void RemoveAll() { m_resources.clear(); }

        /// <summary>
        /// Retrieves or loads a resource using the filename as both ID and name.
        /// This is a convenience method that calls GetWithID using the same string for both parameters.
        /// The resource will be cached using the lowercase version of the name as the key.
        /// </summary>
        /// <typeparam name="T">The specific resource type that must inherit from Resource</typeparam>
        /// <typeparam name="Args">Variadic template for additional constructor arguments</typeparam>
        /// <param name="name">The resource name/filename (used as both ID and load parameter)</param>
        /// <param name="args">Additional arguments forwarded to the resource's Load method</param>
        /// <returns>A shared pointer to the requested resource, or nullptr if loading failed</returns>
        template<typename T, typename ... Args>
            requires std::derived_from<T, Resource>
        res_t<T> Get(const std::string& name, Args&& ... args);

        /// <summary>
        /// Retrieves or loads a resource with separate ID and name parameters.
        /// The ID is used for caching (converted to lowercase), while the name is passed to Load().
        /// This allows loading different files under the same cache key or using custom identifiers.
        /// 
        /// Process:
        /// 1. Check if resource exists in cache using lowercase ID
        /// 2. If found, attempt to cast to requested type T
        /// 3. If not found, create new resource and call Load() with name and args
        /// 4. Cache the successfully loaded resource for future use
        /// </summary>
        /// <typeparam name="T">The specific resource type that must inherit from Resource</typeparam>
        /// <typeparam name="Args">Variadic template for additional constructor arguments</typeparam>
        /// <param name="id">The cache identifier (will be converted to lowercase)</param>
        /// <param name="name">The resource name/filename passed to the Load method</param>
        /// <param name="args">Additional arguments forwarded to the resource's Load method</param>
        /// <returns>A shared pointer to the requested resource, or nullptr if loading failed</returns>
        template<typename T, typename ... Args>
            requires std::derived_from<T, Resource>
        res_t<T> GetWithID(const std::string& id, const std::string& name, Args&& ... args);

    private:
        /// <summary>
        /// Friend declaration to allow Singleton base class access to private constructor
        /// </summary>
        friend class Singleton<ResourceManager>;

        /// <summary>
        /// Private default constructor to enforce singleton pattern.
        /// Only accessible through the Singleton base class Instance() method.
        /// </summary>
        ResourceManager() = default;

    private:
        /// <summary>
        /// Internal cache storing all loaded resources.
        /// Key: Lowercase resource identifier for case-insensitive lookup
        /// Value: Shared pointer to base Resource (stored as base type for type erasure)
        /// 
        /// Resources are stored as base Resource pointers and dynamically cast to specific
        /// types when retrieved, allowing heterogeneous storage in a single container.
        /// </summary>
        std::map<std::string, res_t<Resource>> m_resources;
    };

    /// <summary>
    /// Template implementation for Get() method.
    /// Simple wrapper that forwards to GetWithID using the same string for both parameters.
    /// This provides a convenient interface when the cache key and filename are identical.
    /// </summary>
    template<typename T, typename ... Args>
        requires std::derived_from<T, Resource>
    inline res_t<T> ResourceManager::Get(const std::string& name, Args&& ... args) {
        return GetWithID<T>(name, name, std::forward<Args>(args)...);
    }

    /// <summary>
    /// Template implementation for GetWithID() method.
    /// This is the core resource loading and caching logic.
    /// </summary>
    template<typename T, typename ...Args>
        requires std::derived_from<T, Resource>
    inline res_t<T> ResourceManager::GetWithID(const std::string& id, const std::string& name, Args && ...args) {
        // Normalize the cache key to lowercase for case-insensitive lookup
        std::string key = toLower(id);

        // Check if resource already exists in cache
        auto iter = m_resources.find(key);
        if (iter != m_resources.end()) {
            // Resource found in cache - attempt to cast to requested type
            auto base = iter->second;
            auto resource = std::dynamic_pointer_cast<T>(base);

            // Verify the cast was successful (type safety check)
            if (resource == nullptr) {
                LOG_ERROR("Resource type mismatch: {}", key);
                return res_t<T>();  // Return empty shared_ptr on type mismatch
            }

            // Return the cached resource with correct type
            return resource;
        }

        // Resource not in cache - create and load new instance
        res_t<T> resource = std::make_shared<T>();

        // Attempt to load the resource with provided arguments
        // Perfect forwarding preserves argument value categories
        if (resource->Load(name, std::forward<Args>(args)...) == false) {
            LOG_ERROR("Could not load resource: {}", name);
            return res_t<T>();  // Return empty shared_ptr on load failure
        }

        // Successfully loaded - add to cache for future use
        // Store as base Resource pointer for type erasure
        m_resources[key] = resource;

        return resource;
    }

    /// <summary>
    /// Global convenience function for accessing the ResourceManager singleton.
    /// Provides a shorter, more convenient syntax for resource operations.
    /// 
    /// Usage: Resources().Get&lt;Texture&gt;("player.png") instead of ResourceManager::Instance().Get&lt;Texture&gt;("player.png")
    /// </summary>
    /// <returns>Reference to the ResourceManager singleton instance</returns>
    inline ResourceManager& Resources() { return ResourceManager::Instance(); }
}