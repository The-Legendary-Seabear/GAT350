#pragma once
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
		/// Sets the draw color using 8-bit color values (0-255).
		/// This color is used for drawing primitives and clearing the screen.
		/// </summary>
		/// <param name="r">Red component (0-255)</param>
		/// <param name="g">Green component (0-255)</param>
		/// <param name="b">Blue component (0-255)</param>
		/// <param name="a">Alpha component (0-255), default is 255 (fully opaque)</param>
		void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

		/// <summary>
		/// Sets the draw color using normalized float values (0.0-1.0).
		/// This color is used for drawing primitives and clearing the screen.
		/// </summary>
		/// <param name="r">Red component (0.0-1.0)</param>
		/// <param name="g">Green component (0.0-1.0)</param>
		/// <param name="b">Blue component (0.0-1.0)</param>
		/// <param name="a">Alpha component (0.0-1.0), default is 1.0 (fully opaque)</param>
		void SetColor(float r, float g, float b, float a = 1.0f);

		/// <summary>
		/// Draws a line between two points using the current draw color.
		/// </summary>
		/// <param name="x1">X-coordinate of the starting point</param>
		/// <param name="y1">Y-coordinate of the starting point</param>
		/// <param name="x2">X-coordinate of the ending point</param>
		/// <param name="y2">Y-coordinate of the ending point</param>
		void DrawLine(float x1, float y1, float x2, float y2);

		/// <summary>
		/// Draws a single point at the specified coordinates using the current draw color.
		/// </summary>
		/// <param name="x">X-coordinate of the point</param>
		/// <param name="y">Y-coordinate of the point</param>
		void DrawPoint(float x, float y);

		/// <summary>
		/// Draws a texture at the specified position with its natural size.
		/// The position specifies the top-left corner of the texture.
		/// </summary>
		/// <param name="texture">The texture to draw</param>
		/// <param name="x">X-coordinate of the top-left corner</param>
		/// <param name="y">Y-coordinate of the top-left corner</param>
		void DrawTexture(class Texture& texture, float x, float y);

		/// <summary>
		/// Draws a texture with transformation (rotation, scale, flip).
		/// The position specifies the center of the texture.
		/// </summary>
		/// <param name="texture">The texture to draw</param>
		/// <param name="x">X-coordinate of the center of the texture</param>
		/// <param name="y">Y-coordinate of the center of the texture</param>
		/// <param name="angle">Rotation angle in degrees (clockwise)</param>
		/// <param name="scale">Uniform scale factor (1.0 = normal size)</param>
		/// <param name="flipH">If true, flips the texture horizontally</param>
		void DrawTexture(class Texture& texture, float x, float y, float angle, float scale = 1, bool flipH = false);

		/// <summary>
		/// Draws a portion of a texture (specified by sourceRect) with transformation.
		/// Useful for sprite sheets and texture atlases.
		/// The position specifies the center of the drawn region.
		/// </summary>
		/// <param name="texture">The texture to draw from</param>
		/// <param name="sourceRect">The rectangular region of the texture to draw</param>
		/// <param name="x">X-coordinate of the center of the drawn region</param>
		/// <param name="y">Y-coordinate of the center of the drawn region</param>
		/// <param name="angle">Rotation angle in degrees (clockwise)</param>
		/// <param name="scale">Uniform scale factor (1.0 = normal size)</param>
		/// <param name="flipH">If true, flips the texture horizontally</param>
		void DrawTexture(class Texture& texture, const rect& sourceRect, float x, float y, float angle, float scale = 1, bool flipH = false);

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

		// SDL renderer handle used for all drawing operations
		SDL_Renderer* m_renderer = nullptr;
	};
}
