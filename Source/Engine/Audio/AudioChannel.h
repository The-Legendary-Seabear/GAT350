#pragma once

namespace FMOD { class Channel; }

namespace neu {
	/// <summary>
	/// Represents a single audio channel that can play one sound at a time.
	/// Provides control over playback state, volume, pitch, and looping.
	/// Channels are managed by the AudioSystem and reused when sounds finish playing.
	/// </summary>
	class AudioChannel {
	public:
		AudioChannel() = default;
		~AudioChannel() = default;

		/// <summary>
		/// Updates the channel state. Currently unused but available for future functionality.
		/// </summary>
		void Update();

		/// <summary>
		/// Checks if the channel is currently playing audio.
		/// </summary>
		/// <returns>True if audio is playing on this channel; otherwise, false</returns>
		bool IsPlaying();

		/// <summary>
		/// Stops playback on this channel immediately.
		/// The channel will become available for reuse.
		/// </summary>
		void Stop();

		/// <summary>
		/// Pauses or resumes playback on this channel.
		/// </summary>
		/// <param name="paused">True to pause, false to resume</param>
		void SetPaused(bool paused);

		/// <summary>
		/// Checks if the channel is currently paused.
		/// </summary>
		/// <returns>True if the channel is paused; otherwise, false</returns>
		bool IsPaused() const;

		/// <summary>
		/// Sets the volume level for this channel.
		/// </summary>
		/// <param name="volume">Volume level (0.0 = silent, 1.0 = full volume). Values are clamped to this range.</param>
		void SetVolume(float volume);

		/// <summary>
		/// Gets the current volume level of this channel.
		/// </summary>
		/// <returns>The current volume (0.0 to 1.0)</returns>
		float GetVolume() const;

		/// <summary>
		/// Sets the pitch/frequency multiplier for this channel.
		/// Values greater than 1.0 increase pitch, values less than 1.0 decrease pitch.
		/// </summary>
		/// <param name="pitch">Pitch multiplier (default is 1.0 for normal pitch)</param>
		void SetPitch(float pitch = 1.0f);

		/// <summary>
		/// Gets the current pitch multiplier.
		/// </summary>
		/// <returns>The current pitch value</returns>
		float GetPitch() const;

		/// <summary>
		/// Sets the number of times the sound should loop.
		/// </summary>
		/// <param name="loopCount">Number of loops (-1 for infinite loop, 0 for no loop)</param>
		void SetLoopCount(int loopCount);

	private:
		// Allow AudioSystem to access private members for channel management
		friend class AudioSystem;

		// Flag indicating if this channel is actively being used (currently unused)
		bool m_active{ false };

		// Pointer to the underlying FMOD channel object
		// nullptr when not playing any sound
		FMOD::Channel* m_channel{ nullptr };
	};
}