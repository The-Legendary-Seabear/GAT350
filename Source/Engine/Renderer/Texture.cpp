#include "Texture.h"
#include "Renderer.h"

namespace neu {
    /// <summary>
    /// Destructor ensures proper cleanup of SDL resources.
    /// Destroys the SDL texture if it exists, freeing GPU memory.
    /// </summary>
    Texture::~Texture() {
        // If texture exists, destroy texture to free GPU resources
        if (m_texture) SDL_DestroyTexture(m_texture);
    }

    /// <summary>
    /// Loads an image file from disk and creates an SDL texture for rendering.
    /// The loading process involves two steps:
    /// 1. Load the image into a CPU-side surface using SDL_image
    /// 2. Create a GPU-side texture from the surface for efficient rendering
    /// The surface is freed after texture creation to conserve memory.
    /// </summary>
    /// <param name="filename">Path to the image file to load</param>
    /// <param name="renderer">Reference to the Renderer that provides the SDL_Renderer context</param>
    /// <returns>True if the texture was successfully loaded and created; otherwise, false</returns>
    bool Texture::Load(const std::string& filename, Renderer& renderer) {
        // Load image onto a CPU-side surface
        // SDL_image supports various formats: PNG, JPG, BMP, GIF, etc.
        SDL_Surface* surface = IMG_Load(filename.c_str());
        if (!surface) {
            LOG_ERROR("Could not load image: {}", filename);
            return false;
        }

        // Create a GPU-side texture from the surface
        // The renderer is a friend class, so we can access m_renderer directly
        m_texture = SDL_CreateTextureFromSurface(renderer.m_renderer, surface);

        // Once texture is created on GPU, the CPU-side surface can be freed
        // This saves memory as we only need the GPU texture for rendering
        SDL_DestroySurface(surface);

        if (!m_texture) {
            LOG_ERROR("Could not create texture: {}", filename);
            return false;
        }

        // Query the texture for its dimensions
        SDL_GetTextureSize(m_texture, &m_size.x, &m_size.y);

        return true;
    } 
}