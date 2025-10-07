#pragma once
#include "Math/Math.h"
#include <chrono>

namespace neu {
    /// <summary>
    /// A high-precision timing utility class for tracking elapsed time and frame deltas.
    /// This class provides functionality for measuring time intervals, calculating delta times
    /// between frames, and applying time scaling effects.
    /// </summary>
    class Time {
    private:
        /// <summary>
        /// Type alias for high resolution clock used for precise time measurements
        /// </summary>
        using clock = std::chrono::high_resolution_clock;

    public:
        /// <summary>
        /// Default constructor that initializes the timer with current time as start point.
        /// Both start time and frame time are set to the current high-resolution clock time.
        /// </summary>
        Time() {
            m_startTime = clock::now();
            m_frameTime = clock::now();
        }

        /// <summary>
        /// Updates the timer by calculating current elapsed time and delta time since last frame.
        /// This method should be called once per frame to maintain accurate timing information.
        /// </summary>
        void Tick();

        /// <summary>
        /// Resets the timer by setting the start time to the current time.
        /// This effectively restarts the timer from zero without affecting the frame timing.
        /// </summary>
        void Reset() { m_startTime = clock::now(); }

        /// <summary>
        /// Gets the total elapsed time since the timer was created or last reset.
        /// </summary>
        /// <returns>Total elapsed time in seconds as a floating-point value</returns>
        float GetTime() const { return m_time; }

        /// <summary>
        /// Gets the time elapsed since the last frame, scaled by the current time scale factor.
        /// This is commonly used for frame-rate independent animations and physics calculations.
        /// </summary>
        /// <returns>Delta time in seconds, multiplied by the time scale factor</returns>
        float GetDeltaTime() const { return math::min(m_deltaTime * m_timeScale, m_maxDeltaTime); }

        /// <summary>
        /// Sets the time scale multiplier that affects delta time calculations.
        /// Values greater than 1.0 speed up time, values less than 1.0 slow it down.
        /// A value of 0.0 effectively pauses time-based operations.
        /// </summary>
        /// <param name="timeScale">The scaling factor to apply to delta time (default: 1.0)</param>
        void SetTimeScale(float timeScale) { m_timeScale = timeScale; }

    private:
        /// <summary>
        /// Total elapsed time since timer creation or last reset (in seconds)
        /// </summary>
        float m_time = 0;

        /// <summary>
        /// Time elapsed since the last Tick() call (in seconds, unscaled)
        /// </summary>
        float m_deltaTime = 0;

        /// <summary>
        /// Maximum delta time (in seconds)
        /// </summary>
        float m_maxDeltaTime = 1.0f / 30.0f; // Cap at ~33ms (30 FPS equivalent)

        /// <summary>
        /// Scaling factor applied to delta time calculations (default: 1.0)
        /// </summary>
        float m_timeScale = 1;

        /// <summary>
        /// Time point when the timer was created or last reset
        /// </summary>
        clock::time_point m_startTime;

        /// <summary>
        /// Time point from the previous frame, used for delta time calculations
        /// </summary>
        clock::time_point m_frameTime;
    };
}