#include "Font.h"

namespace neu {
	/// <summary>
	/// Destructor ensures proper cleanup of SDL_ttf resources.
	/// Closes the TrueType font if it exists.
	/// </summary>
	Font::~Font() {
		if (m_ttfFont != nullptr) {
			TTF_CloseFont(m_ttfFont);
		}
	}

	/// <summary>
	/// Loads a TrueType font file using SDL_ttf.
	/// The font can be used to render text at the specified size.
	/// </summary>
	/// <param name="name">Path to the font file (e.g., "assets/fonts/Arial.ttf")</param>
	/// <param name="fontSize">Font size in points (e.g., 24.0f for 24-point font)</param>
	/// <returns>True if the font was successfully loaded; otherwise, false</returns>
	bool Font::Load(const std::string& name, float fontSize) {
		// Open the TrueType font at the specified size
		m_ttfFont = TTF_OpenFont(name.c_str(), fontSize);
		if (m_ttfFont == nullptr) {
			LOG_ERROR("Could not load font: {}", name);
			return false;
		}

		return true;
	}
}