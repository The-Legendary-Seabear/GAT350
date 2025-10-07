#pragma once
#include <fmod.hpp>
#include <string>
#include <map>

namespace neu {
	/// <summary>
	/// Main audio system that manages FMOD initialization, audio playback, and channel management.
	/// This class acts as the central hub for all audio operations in the engine.
	/// It maintains a pool of 32 audio channels that can be reused for playing sounds.
	/// </summary>
	class AudioSystem {
	public:
		AudioSystem() = default;

		/// <summary>
		/// Initializes the FMOD audio system and prepares the channel pool.
		/// Must be called before any audio operations can be performed.
		/// </summary>
		/// <returns>True if initialization was successful; otherwise, false</returns>
		bool Initialize();

		/// <summary>
		/// Shuts down the FMOD audio system and releases all resources.
		/// Should be called during application cleanup.
		/// </summary>
		void Shutdown();

		/// <summary>
		/// Updates the audio system state. Should be called once per frame.
		/// Processes FMOD's internal update loop for streaming, callbacks, and other time-based operations.
		/// </summary>
		void Update();

		/// <summary>
		/// Plays an audio clip on an available channel.
		/// </summary>
		/// <param name="audioClip">The audio clip to play</param>
		/// <param name="volume">Volume level (0.0 to 1.0), default is 1.0 (full volume)</param>
		/// <param name="Pitch">Pitch multiplier for pitch shifting, default is 1 (use original pitch)</param>
		/// <returns>Pointer to the AudioChannel playing the sound, or nullptr if no channels are available</returns>
		AudioChannel* PlaySound(class AudioClip& audioClip, float volume = 1, float pitch = 1);

		/// <summary>
		/// Plays an audio clip on an available channel with infinite looping enabled.
		/// </summary>
		/// <param name="audioClip">The audio clip to play</param>
		/// <param name="volume">Volume level (0.0 to 1.0), default is 1.0 (full volume)</param>
		/// <param name="pitch">Pitch multiplier, default is 1 (use original pitch)</param>
		/// <returns>Pointer to the AudioChannel playing the looped sound, or nullptr if no channels are available</returns>
		AudioChannel* PlaySoundLoop(class AudioClip& audioClip, float volume = 1, float pitch = 1);

	private:
		/// <summary>
		/// Finds and returns the first available (non-playing) channel from the pool.
		/// </summary>
		/// <returns>Pointer to a free AudioChannel, or nullptr if all channels are in use</returns>
		AudioChannel* GetFreeChannel();

		/// <summary>
		/// Checks if an FMOD operation was successful and logs an error if it failed.
		/// </summary>
		/// <param name="result">The FMOD_RESULT value to check</param>
		/// <returns>True if the operation succeeded (FMOD_OK); otherwise, false</returns>
		static bool CheckFMODResult(FMOD_RESULT result);

	private:
		// Allow AudioClip and AudioChannel to access private members
		friend class AudioClip;
		friend class AudioChannel;

		// The core FMOD system object that manages all audio operations
		FMOD::System* m_system = nullptr;

		// Pool of 32 reusable audio channels for efficient sound playback
		// Using unique_ptr for automatic memory management
		std::array<std::unique_ptr<AudioChannel>, 32> m_channels;
	};

}