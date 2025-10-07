#pragma once
#include "Resources/Resource.h"

namespace FMOD { class Sound; }

namespace neu {
	/// <summary>
	/// Represents an audio clip that can be loaded and played through the AudioSystem.
	/// This class wraps an FMOD Sound object and manages its lifetime.
	/// Inherits from Resource to support the resource management system.
	/// </summary>
	class AudioClip : public Resource {
	public:
		AudioClip() = default;
		~AudioClip();

		/// <summary>
		/// Loads an audio file from disk into memory.
		/// </summary>
		/// <param name="filename">Path to the audio file to load</param>
		/// <param name="audioSystem">Reference to the AudioSystem that will manage this clip</param>
		/// <returns>True if the audio file was successfully loaded; otherwise, false</returns>
		bool Load(const std::string& filename, class AudioSystem& audioSystem);

	private:
		// Allow AudioSystem to access private members for playback
		friend class AudioSystem;

		// Pointer to the underlying FMOD sound object
		// Initialized to nullptr and managed throughout the clip's lifetime
		FMOD::Sound* m_sound{ nullptr };
	};
}