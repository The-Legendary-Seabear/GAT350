#pragma once
#include "Framework/Component.h"

namespace neu {
    /// <summary>
    /// Abstract base class for all components that can be rendered to the screen.
    /// This component extends the base Component class to provide rendering capabilities
    /// within the Entity-Component-System (ECS) architecture.
    /// 
    /// All concrete rendering components (sprites, meshes, text, particles, etc.) should
    /// inherit from this class and implement the Draw() method to define their specific
    /// rendering behavior. The component will be automatically called during the render
    /// phase of the game loop.
    /// 
    /// Design pattern: This follows the Strategy pattern, where different rendering
    /// strategies are encapsulated in derived classes while providing a common interface.
    /// </summary>
    class RendererComponent : public Component {
    public:
        /// <summary>
        /// Pure virtual method that must be implemented by all derived rendering components.
        /// This method is called during the render phase to draw the component's visual
        /// representation to the screen.
        /// 
        /// Implementation guidelines:
        /// - Use the provided renderer to submit draw calls
        /// - Access transform information from the parent entity if needed
        /// - Consider the component's enabled state (typically handled by the system)
        /// - Ensure efficient rendering by batching similar draw calls when possible
        /// 
        /// The renderer parameter provides access to low-level drawing functions,
        /// resource management, and rendering state management.
        /// </summary>
        /// <param name="renderer">Reference to the renderer system used for drawing operations.
        /// Forward declared as 'class Renderer' to avoid circular dependencies.</param>
        virtual void Draw(class Renderer& renderer) = 0;
    };
}