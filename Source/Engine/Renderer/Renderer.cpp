#include "Renderer.h"
#include "Texture.h"

namespace neu {
    /// <summary>
    /// Initializes the SDL video subsystem and the SDL_ttf font library.
    /// This must be called before creating windows or performing any rendering.
    /// </summary>
    /// <returns>True if both SDL and TTF were successfully initialized; otherwise, false</returns>
    bool Renderer::Initialize() {
        // Initialize SDL video subsystem
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            LOG_ERROR("SDL_Init Error: {}", SDL_GetError());
            return false;
        }

        // Initialize SDL_ttf for TrueType font rendering
        if (!TTF_Init()) {
            LOG_ERROR("TTF_Init Error: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    /// <summary>
    /// Shuts down the rendering system and releases all SDL resources.
    /// Quits TTF, destroys the renderer and window, and quits SDL.
    /// Call this during application cleanup.
    /// </summary>
    void Renderer::Shutdown() {
        TTF_Quit();                         // Shutdown SDL_ttf
        SDL_DestroyRenderer(m_renderer);    // Destroy the renderer
        SDL_DestroyWindow(m_window);        // Destroy the window
        SDL_Quit();                         // Shutdown SDL
    }

    /// <summary>
    /// Creates a window and associated renderer with the specified properties.
    /// Sets up VSync for smooth rendering and logical presentation for resolution-independent scaling.
    /// The logical presentation uses letterbox mode to maintain aspect ratio.
    /// </summary>
    /// <param name="name">The window title displayed in the title bar</param>
    /// <param name="width">The logical width of the render area in pixels</param>
    /// <param name="height">The logical height of the render area in pixels</param>
    /// <param name="fullscreen">If true, creates a fullscreen window; otherwise, windowed mode</param>
    /// <returns>True if window and renderer creation succeeded; otherwise, false</returns>
    bool Renderer::CreateWindow(const std::string& name, int width, int height, bool fullscreen) {
        // Store the logical dimensions
        m_width = width;
        m_height = height;

        // Create the SDL window
        m_window = SDL_CreateWindow(name.c_str(), width, height, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
        if (m_window == nullptr) {
            LOG_ERROR("SDL_CreateWindow Error: {}", SDL_GetError());
            SDL_Quit();
            return false;
        }

        // Create the SDL renderer associated with the window
        m_renderer = SDL_CreateRenderer(m_window, NULL);
        if (m_renderer == nullptr) {
            LOG_ERROR("SDL_CreateRenderer Error: {}", SDL_GetError());
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            return false;
        }

        // Enable VSync (vertical sync) to synchronize rendering with monitor refresh rate
        // 1 = VSync on, 0 = VSync off
        SDL_SetRenderVSync(m_renderer, 1);

        // Set up logical presentation for resolution-independent rendering
        // Letterbox mode adds black bars to maintain aspect ratio when window is resized
        SDL_SetRenderLogicalPresentation(m_renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

        return true;
    }

    /// <summary>
    /// Sets the rendering draw color using 8-bit integer values (0-255).
    /// This color is used for drawing primitives and clearing the screen.
    /// </summary>
    /// <param name="r">Red component (0-255)</param>
    /// <param name="g">Green component (0-255)</param>
    /// <param name="b">Blue component (0-255)</param>
    /// <param name="a">Alpha/opacity component (0-255, where 255 is fully opaque)</param>
    void Renderer::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    }

    /// <summary>
    /// Sets the rendering draw color using normalized float values (0.0-1.0).
    /// This color is used for drawing primitives and clearing the screen.
    /// </summary>
    /// <param name="r">Red component (0.0-1.0)</param>
    /// <param name="g">Green component (0.0-1.0)</param>
    /// <param name="b">Blue component (0.0-1.0)</param>
    /// <param name="a">Alpha/opacity component (0.0-1.0, where 1.0 is fully opaque)</param>
    void Renderer::SetColor(float r, float g, float b, float a) {
        SDL_SetRenderDrawColorFloat(m_renderer, r, g, b, a);
    }

    /// <summary>
    /// Draws a line from point (x1, y1) to point (x2, y2) using the current draw color.
    /// </summary>
    /// <param name="x1">X-coordinate of the line's starting point</param>
    /// <param name="y1">Y-coordinate of the line's starting point</param>
    /// <param name="x2">X-coordinate of the line's ending point</param>
    /// <param name="y2">Y-coordinate of the line's ending point</param>
    void Renderer::DrawLine(float x1, float y1, float x2, float y2) {
        SDL_RenderLine(m_renderer, x1, y1, x2, y2);
    }

    /// <summary>
    /// Draws a single pixel at the specified coordinates using the current draw color.
    /// </summary>
    /// <param name="x">X-coordinate of the point</param>
    /// <param name="y">Y-coordinate of the point</param>
    void Renderer::DrawPoint(float x, float y) {
        SDL_RenderPoint(m_renderer, x, y);
    }

    /// <summary>
    /// Draws a texture at the specified position with its natural dimensions.
    /// The position represents the top-left corner of where the texture will be drawn.
    /// </summary>
    /// <param name="texture">The texture to render</param>
    /// <param name="x">X-coordinate of the top-left corner</param>
    /// <param name="y">Y-coordinate of the top-left corner</param>
    void Renderer::DrawTexture(Texture& texture, float x, float y) {
        // Get the natural size of the texture
        vec2 size = texture.GetSize();

        // Set up destination rectangle at the specified position
        SDL_FRect destRect;
        destRect.x = x;
        destRect.y = y;
        destRect.w = size.x;
        destRect.h = size.y;

        // Render the entire texture (NULL source rect) to the destination rectangle
        SDL_RenderTexture(m_renderer, texture.m_texture, NULL, &destRect);
    }

    /// <summary>
    /// Draws a texture with transformations applied (rotation, scale, horizontal flip).
    /// The position represents the center of the texture (useful for rotation).
    /// </summary>
    /// <param name="texture">The texture to render</param>
    /// <param name="x">X-coordinate of the texture's center</param>
    /// <param name="y">Y-coordinate of the texture's center</param>
    /// <param name="angle">Rotation angle in degrees (clockwise)</param>
    /// <param name="scale">Uniform scale multiplier (1.0 = original size, 2.0 = double size)</param>
    /// <param name="flipH">If true, mirrors the texture horizontally</param>
    void Renderer::DrawTexture(Texture& texture, float x, float y, float angle, float scale, bool flipH)
    {
        vec2 size = texture.GetSize();

        // Calculate scaled dimensions
        SDL_FRect destRect;
        destRect.w = size.x * scale;
        destRect.h = size.y * scale;

        // Position the texture so (x, y) is at its center
        // Subtract half width/height to convert from center to top-left
        destRect.x = x - destRect.w * 0.5f;
        destRect.y = y - destRect.h * 0.5f;

        // Render with rotation and optional horizontal flip
        // NULL center point means rotate around the rectangle's center
        SDL_RenderTextureRotated(m_renderer, texture.m_texture, NULL, &destRect, angle, NULL, (flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
    }

    /// <summary>
    /// Draws a portion of a texture (defined by sourceRect) with transformations.
    /// Useful for rendering sprites from a sprite sheet or texture atlas.
    /// The position represents the center of the rendered region.
    /// </summary>
    /// <param name="texture">The texture to render from</param>
    /// <param name="sourceRect">The rectangular region within the texture to draw</param>
    /// <param name="x">X-coordinate of the rendered region's center</param>
    /// <param name="y">Y-coordinate of the rendered region's center</param>
    /// <param name="angle">Rotation angle in degrees (clockwise)</param>
    /// <param name="scale">Uniform scale multiplier (1.0 = original size)</param>
    /// <param name="flipH">If true, mirrors the texture horizontally</param>
    void Renderer::DrawTexture(Texture& texture, const rect& sourceRect, float x, float y, float angle, float scale, bool flipH) {
        // Convert our rect type to SDL_FRect for the source rectangle
        SDL_FRect srcRect;
        srcRect.x = sourceRect.x;
        srcRect.y = sourceRect.y;
        srcRect.w = sourceRect.w;
        srcRect.h = sourceRect.h;

        // Calculate scaled destination dimensions
        SDL_FRect destRect;
        destRect.w = srcRect.w * scale;
        destRect.h = srcRect.h * scale;

        // Position so (x, y) is at the center of the drawn region
        destRect.x = x - destRect.w * 0.5f;
        destRect.y = y - destRect.h * 0.5f;

        // Render the specified source region with rotation and optional flip
        SDL_RenderTextureRotated(m_renderer, texture.m_texture, &srcRect, &destRect, angle, NULL, (flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
    }

    /// <summary>
    /// Clears the entire render target with the current draw color.
    /// Call this at the beginning of each frame to clear the previous frame's contents.
    /// </summary>
    void Renderer::Clear() {
        SDL_RenderClear(m_renderer);
    }

    /// <summary>
    /// Presents the rendered frame to the screen by swapping buffers.
    /// Call this at the end of each frame after all drawing operations are complete.
    /// Uses double buffering: renders to back buffer, then swaps to front buffer for display.
    /// </summary>
    void Renderer::Present() {
        SDL_RenderPresent(m_renderer);
    }
}
