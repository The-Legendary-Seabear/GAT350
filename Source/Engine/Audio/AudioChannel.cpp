#include "AudioChannel.h"
#include <fmod.hpp>

namespace neu {
	/// <summary>
	/// Updates the audio channel state.
	/// Currently not implemented but available for future functionality
	/// such as fade effects or position tracking.
	/// </summary>
	void AudioChannel::Update() {

	}

	/// <summary>
	/// Queries FMOD to determine if this channel is currently playing audio.
	/// A channel with a null FMOD channel pointer is considered not playing.
	/// Handles cases where the channel has finished or been stolen by FMOD.
	/// </summary>
	/// <returns>True if audio is actively playing on this channel; otherwise, false</returns>
	bool AudioChannel::IsPlaying() {
		if (m_channel == nullptr) return false;

		bool playing = false;
		FMOD_RESULT result = m_channel->isPlaying(&playing);

		// If the channel handle is invalid or stolen, it's not playing
		// These errors occur when a sound finishes or FMOD reclaims the channel
		if (result == FMOD_ERR_INVALID_HANDLE || result == FMOD_ERR_CHANNEL_STOLEN) {
			m_channel = nullptr;  // Clear the invalid channel pointer
			return false;
		}

		// Check for other errors
		AudioSystem::CheckFMODResult(result);

		return playing;
	}

	/// <summary>
	/// Immediately stops playback on this channel.
	/// After calling Stop, the channel becomes available for reuse.
	/// Asserts if the channel pointer is null.
	/// </summary>
	void AudioChannel::Stop() {
		ASSERT_MSG(m_channel != nullptr, "Audio channel is null");

		FMOD_RESULT result = m_channel->stop();
		AudioSystem::CheckFMODResult(result);
	}

	/// <summary>
	/// Pauses or resumes playback on this channel.
	/// When paused, the sound position is maintained and can be resumed later.
	/// Asserts if the channel pointer is null.
	/// </summary>
	/// <param name="paused">True to pause playback, false to resume</param>
	void AudioChannel::SetPaused(bool paused) {
		ASSERT_MSG(m_channel != nullptr, "Audio channel is null");

		FMOD_RESULT result = m_channel->setPaused(paused);
		AudioSystem::CheckFMODResult(result);
	}

	/// <summary>
	/// Checks if the channel is currently in a paused state.
	/// Asserts if the channel pointer is null.
	/// </summary>
	/// <returns>True if the channel is paused; otherwise, false</returns>
	bool AudioChannel::IsPaused() const {
		ASSERT_MSG(m_channel != nullptr, "Audio channel is null");

		bool paused = false;
		FMOD_RESULT result = m_channel->getPaused(&paused);
		AudioSystem::CheckFMODResult(result);

		return paused;
	}

	/// <summary>
	/// Sets the volume level for this channel.
	/// The volume is clamped to the range [0.0, 1.0] to ensure valid values.
	/// Asserts if the channel pointer is null.
	/// </summary>
	/// <param name="volume">Desired volume level (0.0 = silent, 1.0 = full volume)</param>
	void AudioChannel::SetVolume(float volume) {
		ASSERT_MSG(m_channel != nullptr, "Audio channel is null");

		// Clamp volume to valid range to prevent audio distortion or errors
		volume = math::clamp(volume, 0.0f, 1.0f);
		FMOD_RESULT result = m_channel->setVolume(volume);
		AudioSystem::CheckFMODResult(result);
	}

	/// <summary>
	/// Retrieves the current volume level of this channel.
	/// Asserts if the channel pointer is null.
	/// </summary>
	/// <returns>The current volume level (0.0 to 1.0), or -1 if retrieval fails</returns>
	float AudioChannel::GetVolume()	const {
		ASSERT_MSG(m_channel != nullptr, "Audio channel is null");

		float volume = -1;
		FMOD_RESULT result = m_channel->getVolume(&volume);
		AudioSystem::CheckFMODResult(result);

		return volume;
	}

	/// <summary>
	/// Sets the pitch/frequency multiplier for this channel.
	/// Values above 1.0 increase pitch and playback speed.
	/// Values below 1.0 decrease pitch and playback speed.
	/// Asserts if the channel pointer is null.
	/// </summary>
	/// <param name="frequency">Pitch multiplier (1.0 = normal pitch)</param>
	void AudioChannel::SetPitch(float frequency) {
		ASSERT_MSG(m_channel != nullptr, "Audio channel is null");

		FMOD_RESULT result = m_channel->setPitch(frequency);
		AudioSystem::CheckFMODResult(result);
	}

	/// <summary>
	/// Retrieves the current pitch multiplier for this channel.
	/// Asserts if the channel pointer is null.
	/// </summary>
	/// <returns>The current pitch value, or 0 if retrieval fails</returns>
	float AudioChannel::GetPitch() const {
		ASSERT_MSG(m_channel != nullptr, "Audio channel is null");

		float pitch = 0;
		FMOD_RESULT result = m_channel->getPitch(&pitch);
		AudioSystem::CheckFMODResult(result);

		return pitch;
	}

	/// <summary>
	/// Sets the number of times the audio should loop.
	/// Asserts if the channel pointer is null.
	/// </summary>
	/// <param name="loopCount">Number of loops (-1 = infinite loop, 0 = play once, >0 = loop N times)</param>
	void AudioChannel::SetLoopCount(int loopCount) {
		ASSERT_MSG(m_channel != nullptr, "Audio channel is null");

		FMOD_RESULT result = m_channel->setLoopCount(loopCount);
		AudioSystem::CheckFMODResult(result);
	}
}