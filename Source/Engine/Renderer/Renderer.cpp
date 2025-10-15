#include "Renderer.h"
#include "Texture.h"

namespace neu {

    SDL_GLContext m_context;
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
        //SDL_DestroyRenderer(m_context);    // Destroy the renderer
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
        m_window = SDL_CreateWindow(name.c_str(), width, height, SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
        if (m_window == nullptr) {
            LOG_ERROR("SDL_CreateWindow Error: {}", SDL_GetError());
            SDL_Quit();
            return false;
        }

        // OpenGL
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetSwapInterval(1);

        m_context = SDL_GL_CreateContext(m_window);
        if (m_context == nullptr) {
            LOG_ERROR("SDL_CreateWindow Error: {}", SDL_GetError());
            SDL_Quit();
            return false;
        }
        gladLoadGL();

        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        return true;
    }

   

    /// <summary>
    /// Clears the entire render target with the current draw color.
    /// Call this at the beginning of each frame to clear the previous frame's contents.
    /// </summary>
    void Renderer::Clear() {
        //SDL_RenderClear(m_renderer);
        glClearColor(0.0f, 0.0f, 0.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    /// <summary>
    /// Presents the rendered frame to the screen by swapping buffers.
    /// Call this at the end of each frame after all drawing operations are complete.
    /// Uses double buffering: renders to back buffer, then swaps to front buffer for display.
    /// </summary>
    void Renderer::Present() {
        //SDL_RenderPresent(m_renderer);
        SDL_GL_SwapWindow(m_window);
    }
}
