#include "Texture.h"
#include "Renderer.h"

namespace neu {
    /// <summary>
    /// Destructor ensures proper cleanup of SDL resources.
    /// Destroys the SDL texture if it exists, freeing GPU memory.
    /// </summary>
    Texture::~Texture() {
        // If texture exists, destroy texture to free GPU resources
        if (m_texture) glDeleteTextures(1, &m_texture);
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
    bool Texture::Load(const std::string& filename) {
        // Load image onto a CPU-side surface
        // SDL_image supports various formats: PNG, JPG, BMP, GIF, etc.
        SDL_Surface* surface = IMG_Load(filename.c_str());
        if (!surface) {
            LOG_ERROR("Could not load image: {}", filename);
            return false;
        }

        SDL_FlipSurface(surface, SDL_FLIP_VERTICAL);

        const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(surface->format);

        int channels = details->bytes_per_pixel;
        GLenum internalFormat = (channels == 4) ? GL_RGBA8 : GL_RGB8;
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        glGenTextures(1, &m_texture);
        glBindTexture(m_target, m_texture);

        glTexImage2D(m_target, 0, internalFormat, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

        // Texture parameters
        glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glActiveTexture(GL_TEXTURE0);

        SDL_DestroySurface(surface);

        return true;
    } 
}