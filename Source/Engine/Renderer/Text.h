#pragma once
#include "../Math/Vector3.h"
#include <string>
#include <memory>

struct SDL_Texture;

namespace neu {
	/// <summary>
	/// Represents rendered text that can be drawn to the screen.
	/// This class uses SDL_ttf to render text from a Font into a texture.
	/// The text is rendered once and cached as a texture for efficient drawing.
	/// Note: This is not a Resource - each Text instance is created dynamically as needed.
	/// </summary>
	class Text {
	public:
		Text() = default;

		/// <summary>
		/// Constructs a Text object with an associated font.
		/// </summary>
		/// <param name="font">Shared pointer to the Font to use for rendering</param>
		Text(std::shared_ptr<class Font> font) : m_font{ font } {}

		~Text();

		/// <summary>
		/// Creates a texture from the given text string and color.
		/// The text is rendered using SDL_ttf and converted to a GPU texture.
		/// Call this method whenever the text content or color needs to change.
		/// </summary>
		/// <param name="renderer">Reference to the Renderer used to create the texture</param>
		/// <param name="text">The text string to render</param>
		/// <param name="color">The RGB color of the text (vec3 components in range [0, 1])</param>
		/// <returns>True if the texture was successfully created; otherwise, false</returns>
		bool Create(class Renderer& renderer, const std::string& text, const vec3& color);

		/// <summary>
		/// Draws the text texture at the specified screen position.
		/// The text is drawn at its natural size based on the font size used.
		/// </summary>
		/// <param name="renderer">The Renderer used to draw the text</param>
		/// <param name="x">The x-coordinate (left edge) where the text will be drawn</param>
		/// <param name="y">The y-coordinate (top edge) where the text will be drawn</param>
		void Draw(class Renderer& renderer, float x, float y);

	private:
		// Shared pointer to the Font used for rendering this text
		// Shared ownership allows multiple Text objects to use the same Font
		std::shared_ptr<class Font> m_font{ nullptr };

		// GPU-side texture containing the rendered text
		// Created in Create() and used in Draw()
		SDL_Texture* m_texture{ nullptr };
	};
}
