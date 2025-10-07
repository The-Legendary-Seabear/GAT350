#include "AudioSystem.h"
#include "AudioClip.h"

namespace neu {
	/// <summary>
	/// Checks if an FMOD operation was successful and logs an error message if it was not.
	/// This helper function centralizes error handling for all FMOD calls.
	/// </summary>
	/// <param name="result">The FMOD_RESULT value returned by an FMOD function call</param>
	/// <returns>True if the FMOD operation was successful (FMOD_OK); otherwise, false</returns>
	bool AudioSystem::CheckFMODResult(FMOD_RESULT result) {
		if (result != FMOD_OK) {
			LOG_ERROR("FMOD error {}", FMOD_ErrorString(result));
			return false;
		}

		return true;
	}

	/// <summary>
	/// Searches the channel pool for an available channel.
	/// A channel is considered free if it's not currently playing audio.
	/// </summary>
	/// <returns>Pointer to the first free channel found, or nullptr if all channels are busy</returns>
	AudioChannel* AudioSystem::GetFreeChannel()
	{
		for (auto& channel : m_channels) {
			if (!channel->IsPlaying()) return channel.get();
		}

		return nullptr;
	}

	/// <summary>
	/// Initializes the audio system using FMOD.
	/// Creates the FMOD system object, initializes it with 32 virtual channels,
	/// and prepares the channel pool for use.
	/// </summary>
	/// <returns>True if the audio system was successfully initialized; otherwise, false</returns>
	bool AudioSystem::Initialize() {
		// Create the FMOD system object
		FMOD_RESULT result = FMOD::System_Create(&m_system);
		if (!CheckFMODResult(result)) return false;

		// Initialize FMOD with 32 virtual channels and normal initialization flags
		// extradriverdata is not needed for standard initialization
		void* extradriverdata = nullptr;
		result = m_system->init(32, FMOD_INIT_NORMAL, extradriverdata);
		if (!CheckFMODResult(result)) return false;

		// Initialize the channel pool
		// Each channel is created as a unique_ptr for automatic memory management
		for (auto& channel : m_channels) {
			channel = std::make_unique<AudioChannel>();
		}

		return true;
	}

	/// <summary>
	/// Shuts down the audio system and releases associated resources.
	/// This will stop all playing sounds and clean up FMOD's internal state.
	/// </summary>
	void AudioSystem::Shutdown() {
		CheckFMODResult(m_system->release());
	}

	/// <summary>
	/// Updates the audio system state by processing pending audio operations.
	/// This should be called once per frame to handle streaming, callbacks,
	/// and other time-dependent audio operations.
	/// </summary>
	void AudioSystem::Update() {
		CheckFMODResult(m_system->update());
	}

	/// <summary>
	/// Plays an audio clip on an available channel from the pool.
	/// If no channels are available, a warning is logged and nullptr is returned.
	/// The sound starts playing immediately (not paused).
	/// </summary>
	/// <param name="audioClip">The audio clip containing the sound to play</param>
	/// <param name="volume">Initial volume (0.0 to 1.0), clamped in SetVolume</param>
	/// <param name="Pitch">Pitch multiplier for pitch control (0 = use default pitch)</param>
	/// <returns>Pointer to the AudioChannel playing the sound, or nullptr if playback failed</returns>
	AudioChannel* AudioSystem::PlaySound(AudioClip& audioClip, float volume, float pitch) {
		// Find an available channel from the pool
		AudioChannel* channel = GetFreeChannel();
		if (channel == nullptr) {
			LOG_WARNING("No free channels to play sound");
			return nullptr;
		}

		// Play the sound on the channel
		// Parameters: sound object, channel group (0 = master), start paused (false), output channel pointer
		FMOD_RESULT result = m_system->playSound(audioClip.m_sound, 0, false, &channel->m_channel);
		if (!CheckFMODResult(result)) return nullptr;

		// Apply the requested volume and frequency settings
		channel->SetVolume(volume);
		channel->SetPitch(pitch);

		return channel;
	}

	/// <summary>
	/// Plays an audio clip with infinite looping enabled.
	/// If no channels are available, a warning is logged and nullptr is returned.
	/// The sound starts playing immediately with loop count set to -1 (infinite).
	/// </summary>
	/// <param name="audioClip">The audio clip containing the sound to loop</param>
	/// <param name="volume">Initial volume (0.0 to 1.0), clamped in SetVolume</param>
	/// <param name="pitch">Pitch multiplier for pitch control (1 = use default pitch)</param>
	/// <returns>Pointer to the AudioChannel playing the looped sound, or nullptr if playback failed</returns>
	/// <summary>
	/// Plays an audio clip with infinite looping enabled.
	/// If no channels are available, a warning is logged and nullptr is returned.
	/// The sound starts playing immediately with loop count set to -1 (infinite).
	/// Sets the sound's loop mode to enable looping behavior.
	/// </summary>
	/// <param name="audioClip">The audio clip containing the sound to loop</param>
	/// <param name="volume">Initial volume (0.0 to 1.0), clamped in SetVolume</param>
	/// <param name="pitch">Pitch multiplier for pitch control (0 = use default pitch)</param>
	/// <returns>Pointer to the AudioChannel playing the looped sound, or nullptr if playback failed</returns>
	AudioChannel* AudioSystem::PlaySoundLoop(AudioClip& audioClip, float volume, float pitch) {
		// Find an available channel from the pool
		AudioChannel* channel = GetFreeChannel();
		if (channel == nullptr) {
			LOG_WARNING("No free channels to play sound");
			return nullptr;
		}

		// Set the sound to loop mode before playing
		FMOD_RESULT result = audioClip.m_sound->setMode(FMOD_LOOP_NORMAL);
		if (!CheckFMODResult(result)) return nullptr;

		// Play the sound on the channel
		// Parameters: sound object, channel group (0 = master), start paused (false), output channel pointer
		result = m_system->playSound(audioClip.m_sound, 0, false, &channel->m_channel);
		if (!CheckFMODResult(result)) return nullptr;

		// Apply the requested volume and pitch settings
		channel->SetVolume(volume);
		channel->SetPitch(pitch);

		// Set infinite looping (-1 means loop forever)
		channel->SetLoopCount(-1);

		return channel;
	}
}