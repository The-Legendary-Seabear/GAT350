#pragma once

namespace neu {
	class Texture;

	/// <summary>
	/// Represents a sprite sheet animation that divides a texture into a grid of frames.
	/// This class manages frame-based animation data including timing, layout, and looping behavior.
	/// Animation data is loaded from a JSON file that specifies the sprite sheet layout and playback settings.
	/// Inherits from Resource to support the resource management system.
	/// </summary>
	class TextureAnimation : public Resource {
	public:
		TextureAnimation() = default;
		~TextureAnimation() = default;

		/// <summary>
		/// Loads animation data from a JSON file and the associated sprite sheet texture.
		/// The JSON file should contain: texture_name, columns, rows, start_frame, 
		/// total_frames, frames_per_second, and loop settings.
		/// </summary>
		/// <param name="filename">Path to the animation JSON file (e.g., "assets/animations/player_walk.json")</param>
		/// <param name="renderer">Reference to the Renderer used to load the texture</param>
		/// <returns>True if the animation was successfully loaded; otherwise, false</returns>
		bool Load(const std::string& filename, class Renderer& renderer);

		/// <summary>
		/// Gets the size of a single animation frame in pixels.
		/// Calculated by dividing the texture size by the number of columns and rows.
		/// </summary>
		/// <returns>A vec2 containing the frame width (x) and height (y)</returns>
		vec2 GetSize() const;

		/// <summary>
		/// Gets the rectangle (UV coordinates) for a specific frame in the sprite sheet.
		/// Used to determine which portion of the texture to render for the given frame.
		/// </summary>
		/// <param name="frame">The frame index (0-based, relative to start_frame)</param>
		/// <returns>A rect containing the x, y position and width, height of the frame</returns>
		rect GetFrameRect(int frame) const;

		/// <summary>
		/// Gets the total number of frames in the animation sequence.
		/// </summary>
		/// <returns>The total frame count</returns>
		int GetTotalFrames() const { return m_totalFrames; }

		/// <summary>
		/// Gets the playback speed of the animation.
		/// </summary>
		/// <returns>Frames per second (FPS) rate</returns>
		float GetFPS() const { return m_framesPerSecond; }

		/// <summary>
		/// Checks if the given frame index is within valid bounds.
		/// </summary>
		/// <param name="frame">The frame index to validate</param>
		/// <returns>True if the frame is valid (0 <= frame < total_frames); otherwise, false</returns>
		bool IsValidFrame(int frame) const { return frame >= 0 && frame < m_totalFrames; }

		/// <summary>
		/// Checks if the animation should loop back to the beginning after completion.
		/// </summary>
		/// <returns>True if the animation loops; otherwise, false</returns>
		bool IsLooping() const { return m_loop; }

		/// <summary>
		/// Gets the underlying texture resource containing the sprite sheet.
		/// </summary>
		/// <returns>Resource pointer to the Texture</returns>
		res_t<Texture> GetTexture() { return m_texture; }

	private:
		// Number of columns in the sprite sheet grid
		int m_columns = 0;

		// Number of rows in the sprite sheet grid
		int m_rows = 0;

		// The starting frame index in the sprite sheet (for multi-animation sheets)
		int m_startFrame = 0;

		// Total number of frames in this animation sequence
		int m_totalFrames = 0;

		// Playback speed in frames per second
		float m_framesPerSecond = 0;

		// Whether the animation should loop continuously
		bool m_loop = true;

		// Resource pointer to the sprite sheet texture
		res_t<Texture> m_texture;
	};
}
