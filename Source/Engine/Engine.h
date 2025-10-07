#pragma once
#include "Core/Time.h"
#include "Core/Singleton.h"

#include <memory>

namespace neu {
    class Renderer;
    class AudioSystem;
    class InputSystem;

    /// <summary>
    /// Core engine class that manages all major subsystems and provides the main application framework.
    /// 
    /// The Engine class serves as the central orchestrator for the game engine, responsible for:
    /// - Initializing and managing core subsystems (rendering, audio, input)
    /// - Providing the main update loop coordination
    /// - Managing engine lifecycle (startup, update, shutdown)
    /// - Offering global access to system components
    /// - Coordinating resource cleanup and system dependencies
    /// 
    /// Architecture:
    /// - Uses the Singleton pattern for global engine access
    /// - Manages subsystems through unique_ptr for automatic memory management
    /// - Provides a centralized timing system for frame-rate independent updates
    /// - Follows RAII principles for reliable resource management
    /// 
    /// Typical Usage:
    /// ```cpp
    /// Engine& engine = GetEngine();
    /// if (engine.Initialize()) {
    ///     while (running) {
    ///         engine.Update();
    ///         engine.Draw();
    ///     }
    ///     engine.Shutdown();
    /// }
    /// ```
    /// 
    /// The engine ensures proper initialization order and handles cleanup of all
    /// managed resources, making it safe to use in both simple applications and
    /// complex game scenarios.
    /// </summary>
    class Engine : public Singleton<Engine> {
    public:
        /// <summary>
        /// Initializes all engine subsystems in the correct dependency order.
        /// 
        /// Initialization sequence:
        /// 1. Creates and initializes the rendering system with default window
        /// 2. Sets up the input system for user interaction handling
        /// 3. Initializes the audio system for sound playback
        /// 
        /// The method ensures that dependencies between systems are respected
        /// and that all systems are properly configured before returning.
        /// If any system fails to initialize, the entire initialization fails.
        /// </summary>
        /// <returns>True if all systems initialized successfully, false if any system failed</returns>
        bool Initialize();

        /// <summary>
        /// Shuts down all engine systems and releases resources in reverse initialization order.
        /// 
        /// Shutdown sequence:
        /// 1. Clears all cached resources from ResourceManager
        /// 2. Removes all registered factories from Factory system
        /// 3. Clears all event handlers from EventManager
        /// 4. Shuts down audio system (releases audio devices and buffers)
        /// 5. Shuts down input system (releases input devices)
        /// 6. Shuts down renderer (destroys OpenGL context and window)
        /// 
        /// This method ensures graceful cleanup and prevents resource leaks.
        /// It should be called before application termination.
        /// </summary>
        void Shutdown();

        /// <summary>
        /// Updates all engine systems for the current frame.
        /// 
        /// Update sequence:
        /// 1. Updates the timing system (calculates delta time)
        /// 2. Processes input events and updates input state
        /// 3. Updates audio system (processes sound effects and music)
        /// 
        /// This method should be called once per frame in the main game loop.
        /// It coordinates the update of all time-dependent systems and ensures
        /// they receive consistent timing information.
        /// </summary>
        void Update();

        /// <summary>
        /// Performs rendering operations for the current frame.
        /// 
        /// Currently empty - derived applications or game-specific code should
        /// override or extend this method to perform actual rendering operations.
        /// This method is called after Update() in the typical game loop pattern.
        /// 
        /// Expected usage in derived classes:
        /// - Clear the screen
        /// - Render game objects
        /// - Present the final frame to the display
        /// </summary>
        void Draw();

        /// <summary>
        /// Provides access to the rendering system for graphics operations.
        /// The renderer handles window management, OpenGL context, and drawing operations.
        /// </summary>
        /// <returns>Reference to the active Renderer instance</returns>
        Renderer& GetRenderer() { return *m_renderer; }

        /// <summary>
        /// Provides access to the audio system for sound playback and management.
        /// The audio system handles sound effects, background music, and audio device management.
        /// </summary>
        /// <returns>Reference to the active AudioSystem instance</returns>
        AudioSystem& GetAudio() { return *m_audio; }

        /// <summary>
        /// Provides access to the input system for user interaction handling.
        /// The input system processes keyboard, mouse, and other input device events.
        /// </summary>
        /// <returns>Reference to the active InputSystem instance</returns>
        InputSystem& GetInput() { return *m_input; }

        /// <summary>
        /// Provides access to the timing system for frame-rate independent operations.
        /// The time system provides delta time, total elapsed time, and time scaling functionality.
        /// Essential for smooth animations and consistent game behavior across different frame rates.
        /// </summary>
        /// <returns>Reference to the engine's Time system</returns>
        Time& GetTime() { return m_time; }


        void SetScene(std::unique_ptr<Scene> scene) { m_scene = std::move(scene); }

    private:
        /// <summary>
        /// Friend declaration allowing Singleton base class access to private constructor.
        /// This is required for the Singleton pattern implementation.
        /// </summary>
        friend class Singleton<Engine>;

        /// <summary>
        /// Private default constructor enforcing singleton pattern.
        /// Only accessible through Singleton::Instance() method.
        /// Initializes the engine in an uninitialized state - Initialize() must be called.
        /// </summary>
        Engine() = default;

    private:
        /// <summary>
        /// High-precision timing system providing delta time and elapsed time information.
        /// Used by all time-dependent systems for consistent frame-rate independent behavior.
        /// Updated once per frame in Update() method.
        /// </summary>
        Time m_time;

        /// <summary>
        /// Rendering system responsible for graphics output and window management.
        /// Handles OpenGL context creation, window management, and all drawing operations.
        /// Managed through unique_ptr for automatic cleanup and clear ownership semantics.
        /// </summary>
        std::unique_ptr<Renderer> m_renderer;

        /// <summary>
        /// Audio system managing sound playback, music, and audio device interfaces.
        /// Handles audio resource loading, 3D positional audio, and audio mixing.
        /// Managed through unique_ptr for exception-safe resource management.
        /// </summary>
        std::unique_ptr<AudioSystem> m_audio;

        /// <summary>
        /// Input system processing user interactions from keyboard, mouse, and other devices.
        /// Provides event-based and polling-based input access with customizable key mappings.
        /// Managed through unique_ptr for consistent lifecycle management.
        /// </summary>
        std::unique_ptr<InputSystem> m_input;


        std::unique_ptr<Scene> m_scene;
    };

    /// <summary>
    /// Global convenience function for accessing the Engine singleton instance.
    /// Provides a shorter, more readable alternative to Engine::Instance().
    /// 
    /// This function is the preferred way to access the engine from application code,
    /// offering better readability and consistency across the codebase.
    /// 
    /// Usage: GetEngine().GetRenderer().ClearScreen();
    /// </summary>
    /// <returns>Reference to the singleton Engine instance</returns>
    inline Engine& GetEngine() { return Engine::Instance(); }
}