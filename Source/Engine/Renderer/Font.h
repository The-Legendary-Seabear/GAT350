#pragma once
#include "Resources/Resource.h"
#include <string>

struct TTF_Font;

namespace neu {
	/// <summary>
	/// Represents a TrueType font that can be used for text rendering.
	/// This class wraps an SDL_ttf font object and manages its lifetime.
	/// Inherits from Resource to support the resource management system.
	/// </summary>
	class Font : public Resource {
	public:
		Font() = default;
		~Font();

		/// <summary>
		/// Loads a TrueType font from a file at the specified size.
		/// </summary>
		/// <param name="name">Path to the TrueType font file (.ttf or .otf)</param>
		/// <param name="fontSize">Size of the font in points</param>
		/// <returns>True if the font was successfully loaded; otherwise, false</returns>
		bool Load(const std::string& name, float fontSize);

	private:
		// Allow Text class to access the font for rendering
		friend class Text;

		// Pointer to the underlying SDL_ttf font object
		// Initialized to nullptr and managed throughout the font's lifetime
		TTF_Font* m_ttfFont{ nullptr };
	};
}