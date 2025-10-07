#include "AudioClip.h"
#include "AudioSystem.h"

namespace neu {
    /// <summary>
    /// Destructor ensures proper cleanup of FMOD resources.
    /// Releases the FMOD sound object if it exists.
    /// </summary>
    AudioClip::~AudioClip() {
        if (m_sound) m_sound->release();
    }

    /// <summary>
    /// Loads an audio file from disk using FMOD.
    /// If a sound is already loaded, it will be released before loading the new one.
    /// </summary>
    /// <param name="filename">Path to the audio file (supports various formats: WAV, MP3, OGG, etc.)</param>
    /// <param name="audioSystem">Reference to the AudioSystem that provides the FMOD system object</param>
    /// <returns>True if the audio file was successfully loaded; otherwise, false</returns>
    bool AudioClip::Load(const std::string& filename, AudioSystem& audioSystem) {
        // Release existing sound if present to prevent memory leaks
        if (m_sound) {
            m_sound->release();
            m_sound = nullptr;
        }

        // Create the FMOD sound object using default settings
        // FMOD_DEFAULT loads the sound into memory and decompresses it
        FMOD_RESULT result = audioSystem.m_system->createSound(filename.c_str(), FMOD_DEFAULT, 0, &m_sound);
        if (!AudioSystem::CheckFMODResult(result)) return false;

        return true;
    }
}
