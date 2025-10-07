#include "Engine.h"
#include "Renderer/Renderer.h"
#include "Input/InputSystem.h"
#include "Audio/AudioSystem.h"

namespace neu {

    /// <summary>
    /// Initializes all engine subsystems in dependency order.
    /// Sets up the core systems required for a functional game engine.
    /// </summary>
    bool Engine::Initialize() {
        // Initialize the rendering system first as other systems may depend on it
        m_renderer = std::make_unique<neu::Renderer>();
        m_renderer->Initialize();

        // Create the main application window with default settings
        // Window title: "Computer Graphics", Resolution: 1280x1024, Windowed mode
        m_renderer->CreateWindow("Computer Graphics", 1280, 1024, false);

        // Initialize input system for handling user interactions
        // Must be initialized after renderer as it may need window context
        m_input = std::make_unique<neu::InputSystem>();
        m_input->Initialize();

        // Initialize audio system for sound playback capabilities
        // Independent of other systems, can be initialized last
        m_audio = std::make_unique<neu::AudioSystem>();
        m_audio->Initialize();

        // Return true indicating successful initialization
        // In a more robust implementation, this would check each system's
        // initialization status and return false if any system fails
        return true;
    }

    /// <summary>
    /// Performs complete engine shutdown with proper resource cleanup.
    /// Shuts down systems in reverse order of initialization to respect dependencies.
    /// </summary>
    void Engine::Shutdown() {
        // Clear all cached resources to prevent memory leaks
        // This releases textures, sounds, models, and other loaded assets
        Resources().RemoveAll();

        // Clear factory registry to remove all registered object creators
        // Prevents dangling pointers and ensures clean factory state
        Factory::Instance().RemoveAll();

        // Remove all event handlers to prevent callbacks to destroyed objects
        // Essential for preventing crashes during shutdown
        EventManager::Instance().RemoveAll();

        // Shutdown engine systems in reverse initialization order
        // This ensures dependencies are properly handled during cleanup

        // Shutdown audio system first (least dependent on other systems)
        m_audio->Shutdown();

        // Shutdown input system (may depend on window context)
        m_input->Shutdown();

        // Shutdown renderer last (other systems may depend on graphics context)
        m_renderer->Shutdown();


        if (m_scene) m_scene->Destroyed();

        // unique_ptr destructors will automatically clean up allocated memory
        // when the Engine instance is destroyed
    }

    /// <summary>
    /// Updates all time-dependent engine systems for the current frame.
    /// Maintains consistent update order for predictable system behavior.
    /// </summary>
    void Engine::Update() {
        // Update timing system first to provide accurate delta time
        // This calculates the time elapsed since the last frame
        // All other systems depend on this timing information
        m_time.Tick();

        // Update input system to process new events and update input state
        // This captures keyboard presses, mouse movements, controller input, etc.
        // Must happen early in the update cycle for responsive input handling
        m_input->Update();

        // Update audio system to process sound effects and music
        // This handles audio streaming, 3D positional audio updates,
        // and removes finished sound effects from memory
        m_audio->Update();

        // Note: Game-specific update logic (entities, physics, AI, etc.)
        // would typically happen here in derived classes or through
        // additional systems managed by the engine


        if (m_scene) m_scene->Update(m_time.GetDeltaTime());
    }

    void Engine::Draw()
    {
        if (m_scene) m_scene->Draw(*m_renderer);
    }
}