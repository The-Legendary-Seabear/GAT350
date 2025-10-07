#include "TextureAnimation.h"

namespace neu {
	/// <summary>
	/// Loads animation data from a JSON configuration file.
	/// The JSON file defines the sprite sheet layout and animation settings.
	/// Expected JSON format:
	/// {
	///   "texture_name": "path/to/spritesheet.png",
	///   "columns": 4,
	///   "rows": 2,
	///   "start_frame": 0,
	///   "total_frames": 8,
	///   "frames_per_second": 10.0,
	///   "loop": true
	/// }
	/// </summary>
	/// <param name="filename">Path to the JSON animation configuration file</param>
	/// <param name="renderer">Reference to the Renderer for loading the texture</param>
	/// <returns>True if the animation and texture were successfully loaded; otherwise, false</returns>
	bool TextureAnimation::Load(const std::string& filename, class Renderer& renderer) {
		// Load the JSON document
		neu::serial::document_t document;
		if (!neu::serial::Load(filename, document)) {
			LOG_ERROR("Could not load TextureAnimation json {}", filename);
			return false;
		}

		// Read the texture filename from the JSON
		std::string texture_name;
		SERIAL_READ(document, texture_name);

		// Load the sprite sheet texture using the resource manager
		m_texture = Resources().Get<Texture>(texture_name, renderer);
		if (!m_texture) {
			LOG_ERROR("Could not load texture in Texture Animation {}", texture_name);
		}

		// Read animation configuration from JSON
		SERIAL_READ_NAME(document, "columns", m_columns);           // Grid columns
		SERIAL_READ_NAME(document, "rows", m_rows);                 // Grid rows
		SERIAL_READ_NAME(document, "start_frame", m_startFrame);    // Starting frame offset
		SERIAL_READ_NAME(document, "total_frames", m_totalFrames);  // Number of frames in sequence
		SERIAL_READ_NAME(document, "frames_per_second", m_framesPerSecond); // Playback speed
		SERIAL_READ_NAME(document, "loop", m_loop);                 // Loop behavior

		return true;
	}

	/// <summary>
	/// Calculates the size of a single animation frame.
	/// Divides the total texture size by the grid dimensions to get individual frame size.
	/// </summary>
	/// <returns>A vec2 containing the frame width and height in pixels</returns>
	vec2 TextureAnimation::GetSize() const {
		vec2 size = m_texture->GetSize();

		// Calculate frame size by dividing texture dimensions by grid layout
		return { size.x / m_columns, size.y / m_rows };
	}

	/// <summary>
	/// Calculates the rectangle coordinates for a specific frame in the sprite sheet.
	/// Frames are indexed left-to-right, top-to-bottom in the grid.
	/// The calculation accounts for the start_frame offset to support multi-animation sheets.
	/// </summary>
	/// <param name="frame">The frame index (0-based) within the animation sequence</param>
	/// <returns>A rect defining the position and size of the frame in the texture</returns>
	rect TextureAnimation::GetFrameRect(int frame) const
	{
		// Validate frame index and default to frame 0 if out of bounds
		if (!IsValidFrame(frame)) {
			LOG_WARNING("Frame is outside bounds {}/{}", frame, m_totalFrames);
			frame = 0;
		}

		rect frameRect;
		vec2 size = GetSize();

		// Set frame dimensions
		frameRect.w = size.x;
		frameRect.h = size.y;

		// Calculate frame position in the sprite sheet grid
		// Add start_frame offset to support multiple animations in one sheet
		// Use modulo for column (x) and division for row (y) to map 1D index to 2D grid
		frameRect.x = ((m_startFrame + frame) % m_columns) * frameRect.w;
		frameRect.y = ((m_startFrame + frame) / m_columns) * frameRect.h;

		return frameRect;
	}
}