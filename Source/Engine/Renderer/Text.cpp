#include "Text.h"
#include "Font.h"
#include "Renderer.h"

namespace neu {
	/// <summary>
	/// Destroys the Text object and releases its associated SDL texture if it exists.
	/// </summary>
	Text::~Text() {
		if (m_texture != nullptr) {
			SDL_DestroyTexture(m_texture);
		}
	}

	/// <summary>
	/// Creates a texture from the given text string and color using the specified renderer.
	/// </summary>
	/// <param name="renderer">Reference to the Renderer object used to create the texture.</param>
	/// <param name="text">The text string to render.</param>
	/// <param name="color">The color of the text, represented as a vec3 (RGB components in the range [0, 1]).</param>
	/// <returns>True if the texture was successfully created; false otherwise.</returns>
	bool Text::Create(Renderer& renderer, const std::string& text, const vec3& color) {
		// create a surface using the font, text string and color
		

		return true;
	}

	/// <summary>
	/// Draws the text texture at the specified position using the given renderer.
	/// </summary>
	/// <param name="renderer">The renderer used to draw the text.</param>
	/// <param name="x">The x-coordinate where the text will be drawn.</param>
	/// <param name="y">The y-coordinate where the text will be drawn.</param>
	void Text::Draw(Renderer& renderer, float x, float y) {
		
	}
}
