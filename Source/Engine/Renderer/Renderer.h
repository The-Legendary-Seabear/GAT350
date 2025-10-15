#pragma once
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

namespace neu {
	/// <summary>
	/// Main rendering system that manages the SDL window and renderer.
	/// Provides methods for initializing the graphics system, clearing the screen,
	/// drawing primitives (lines, points), rendering textures, and presenting frames.
	/// This class wraps SDL3's rendering functionality with a simplified interface.
	/// </summary>
	class Renderer
	{
	public:
		Renderer() = default;

		

		/// <summary>
		/// Initializes SDL video and TTF (TrueType font) systems.
		/// Must be called before creating a window or performing any rendering operations.
		/// </summary>
		/// <returns>True if initialization was successful; otherwise, false</returns>
		bool Initialize();

		/// <summary>
		/// Shuts down the renderer and cleans up SDL resources.
		/// Destroys the renderer, window, and quits SDL and TTF systems.
		/// Should be called during application cleanup.
		/// </summary>
		void Shutdown();

		/// <summary>
		/// Creates a window with the specified properties and an associated renderer.
		/// Sets up VSync and logical presentation (letterbox scaling) for consistent rendering.
		/// </summary>
		/// <param name="name">The title of the window</param>
		/// <param name="width">The width of the window in pixels</param>
		/// <param name="height">The height of the window in pixels</param>
		/// <param name="fullscreen">If true, creates a fullscreen window; otherwise, windowed mode</param>
		/// <returns>True if the window and renderer were successfully created; otherwise, false</returns>
		bool CreateWindow(const std::string& name, int width, int height, bool fullscreen = false);

		/// <summary>
		/// Clears the screen with the current draw color.
		/// Call this at the beginning of each frame before drawing.
		/// </summary>
		void Clear();

		/// <summary>
		/// Presents the rendered frame to the screen.
		/// Call this at the end of each frame after all drawing is complete.
		/// Swaps the back buffer to the front buffer (double buffering).
		/// </summary>
		void Present();


		/// <summary>
		/// Gets the width of the window/render target.
		/// </summary>
		/// <returns>The width in pixels</returns>
		int GetWidth() const { return m_width; }

		/// <summary>
		/// Gets the height of the window/render target.
		/// </summary>
		/// <returns>The height in pixels</returns>
		int GetHeight() const { return m_height; }

	private:
		// Allow Text and Texture classes to access the SDL renderer for their operations
		friend class Text;
		friend class Texture;

		// Dimensions of the render target
		int m_width{ 0 };
		int m_height{ 0 };

		// SDL window handle
		SDL_Window* m_window = nullptr;

		SDL_GLContext m_context;
	};
}
