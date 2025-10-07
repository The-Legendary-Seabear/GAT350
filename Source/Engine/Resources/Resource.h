#pragma once
#include <memory>

namespace neu {
    /// <summary>
    /// Abstract base class for all managed resources in the engine.
    /// This class provides a common interface for resources that can be loaded,
    /// cached, and managed by the ResourceManager. All specific resource types
    /// (textures, sounds, models, etc.) should inherit from this class.
    /// </summary>
    class Resource {
    public:
        /// <summary>
        /// Default constructor for resource objects.
        /// Resources are typically constructed in an unloaded state and then
        /// populated through their Load() method implementation.
        /// </summary>
        Resource() = default;

        /// <summary>
        /// Virtual destructor to ensure proper cleanup of derived resource types.
        /// This allows ResourceManager to safely delete resources through base pointers
        /// and ensures derived class destructors are called correctly.
        /// </summary>
        virtual ~Resource() = default;
    };

    /// <summary>
    /// Type alias for shared pointer to resource objects.
    /// This provides automatic memory management and reference counting for resources,
    /// allowing multiple systems to safely share the same resource instance.
    /// The template parameter T defaults to Resource but can be specialized for specific types.
    /// 
    /// Usage examples:
    /// - res_t&lt;Texture&gt; myTexture = Resources().Get&lt;Texture&gt;("player.png");
    /// - res_t&lt;Sound&gt; mySound = Resources().Get&lt;Sound&gt;("explosion.wav");
    /// - res_t&lt;&gt; genericResource = Resources().Get&lt;SomeResource&gt;("data.txt");
    /// </summary>
    /// <typeparam name="T">The specific resource type, defaults to base Resource class</typeparam>
    template <typename T = Resource>
        requires std::derived_from<T, Resource>
    using res_t = std::shared_ptr<T>;
}