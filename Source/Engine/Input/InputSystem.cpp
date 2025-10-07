#include "InputSystem.h"

namespace neu {
    /// <summary>
    /// Initializes the input system and captures the initial input state.
    /// 
    /// This method sets up the input system by:
    /// 1. Querying SDL for the current keyboard state
    /// 2. Copying the keyboard state to both current and previous state arrays
    /// 3. Capturing initial mouse position and button states
    /// 
    /// The dual state initialization prevents false input events on the first frame
    /// by ensuring that press/release detection logic has valid previous state data.
    /// 
    /// This method must be called after SDL has been initialized but before any
    /// input queries are performed.
    /// </summary>
    /// <returns>True indicating successful initialization (currently always succeeds)</returns>
    bool InputSystem::Initialize() {
        // Query SDL for the current keyboard state
        // SDL maintains an internal array of boolean values for all scancodes
        const bool* keyboardState = SDL_GetKeyboardState(nullptr);

        // Copy the current SDL keyboard state to our internal array
        // This captures the initial state of all keys at startup
        std::copy(keyboardState, keyboardState + SDL_SCANCODE_COUNT, m_keyboardState.begin());

        // Initialize previous state to current state to prevent false events on first frame
        // Without this, all pressed keys would register as "just pressed" on frame 1
        m_prevKeyboardState = m_keyboardState;

        // Capture initial mouse position directly into our vec2 structure
        // SDL_GetMouseState returns position relative to window client area
        SDL_GetMouseState(&m_mousePosition.x, &m_mousePosition.y);

        // Initialize previous mouse position to prevent false movement delta on first frame
        m_prevMousePosition = m_mousePosition;

        // Always return true as current implementation cannot fail
        // Future versions might add error checking for SDL state
        return true;
    }

    /// <summary>
    /// Shuts down the input system and performs cleanup.
    /// 
    /// Currently this method performs no operations as the InputSystem uses only
    /// stack-allocated data structures that clean up automatically. This method
    /// exists for:
    /// - Consistency with other engine systems that require explicit shutdown
    /// - Future extensibility if resources need to be released
    /// - Maintaining a clear system lifecycle pattern
    /// 
    /// Safe to call multiple times or without prior Initialize() call.
    /// </summary>
    void InputSystem::Shutdown() {
        // Currently no cleanup required as we use only stack-allocated arrays
        // Future implementations might need to:
        // - Release any allocated resources
        // - Unregister from SDL event callbacks
        // - Clear any persistent input state
    }

    /// <summary>
    /// Updates all input state for the current frame.
    /// 
    /// This is the main update method that must be called once per frame, typically
    /// early in the game loop before any input queries. The update process follows
    /// a specific order to ensure consistent state:
    /// 
    /// 1. Save current states as previous states (for transition detection)
    /// 2. Query SDL for new input states
    /// 3. Process and store new states in current state variables
    /// 
    /// This ordering ensures that all input queries during the frame return
    /// consistent results and that press/release detection works correctly.
    /// </summary>
    void InputSystem::Update() {
        // =====================================================================
        // KEYBOARD INPUT UPDATE
        // =====================================================================

        // Save current keyboard state as previous state before updating
        // This enables detection of key press/release events by comparing
        // current frame state with previous frame state
        m_prevKeyboardState = m_keyboardState;

        // Query SDL for the current keyboard state
        // SDL maintains this array internally and updates it based on OS events
        const bool* keyboardState = SDL_GetKeyboardState(nullptr);

        // Copy the entire SDL keyboard state to our internal array
        // This is a fast memory copy operation that updates all key states at once
        std::copy(keyboardState, keyboardState + m_keyboardState.size(), m_keyboardState.begin());

        // =====================================================================
        // MOUSE INPUT UPDATE
        // =====================================================================

        // Save current mouse position as previous position for delta calculation
        // This must happen before querying new position to maintain correct delta
        m_prevMousePosition = m_mousePosition;

        // Save current mouse button states as previous states for press/release detection
        m_prevMouseButtonState = m_mouseButtonState;

        // Query SDL for current mouse state (position and buttons in one call)
        // This is more efficient than separate calls and ensures atomic state
        uint32_t mouseButtonState = SDL_GetMouseState(&m_mousePosition.x, &m_mousePosition.y);

        // Extract individual mouse button states from SDL's button mask
        // SDL returns a bitmask where each bit represents a different button
        // We use bitwise AND to test if each button's bit is set

        // Test left mouse button (bit 0)
        m_mouseButtonState[(uint8_t)MouseButton::Left] = mouseButtonState & SDL_BUTTON_LMASK;

        // Test middle mouse button (bit 1) 
        m_mouseButtonState[(uint8_t)MouseButton::Middle] = mouseButtonState & SDL_BUTTON_MMASK;

        // Test right mouse button (bit 2)
        m_mouseButtonState[(uint8_t)MouseButton::Right] = mouseButtonState & SDL_BUTTON_RMASK;
    }
}