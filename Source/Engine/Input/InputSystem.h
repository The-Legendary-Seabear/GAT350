#pragma once
#include "../Math/Vector2.h"

#include <SDL3/SDL.h>
#include <vector>
#include <array>

namespace neu {
    /// <summary>
    /// Input management system providing centralized handling of keyboard and mouse input.
    /// 
    /// This system wraps SDL3 input functionality and provides a consistent, frame-based
    /// input interface for the game engine. It maintains both current and previous frame
    /// input states, enabling detection of press/release events and input state changes.
    /// 
    /// Key Features:
    /// - Frame-based input state management with current/previous state tracking
    /// - Keyboard input handling using SDL scancodes for layout-independent keys
    /// - Mouse button and position tracking with delta movement calculation
    /// - Event-style input detection (pressed this frame, released this frame)
    /// - Consistent API for both polling and event-based input patterns
    /// 
    /// Design Philosophy:
    /// - Centralizes all input handling in one system for consistency
    /// - Provides both immediate state queries and event-style detection
    /// - Uses fixed-size arrays for optimal performance and memory usage
    /// - Maintains frame coherency by updating all input state together
    /// 
    /// Usage Patterns:
    /// - Query continuous input states (movement, held buttons)
    /// - Detect single-frame events (button presses, releases)
    /// - Track mouse movement and position for camera or UI interaction
    /// - Integrate with game systems that need frame-consistent input data
    /// 
    /// Thread Safety:
    /// - This system should only be updated from the main thread
    /// - Input queries are safe to call from multiple threads after Update()
    /// - SDL input functions are called only during Update() for thread safety
    /// </summary>
    class InputSystem {
    public:
        /// <summary>
        /// Enumeration representing the standard mouse buttons.
        /// Maps to SDL mouse button constants for internal processing while
        /// providing a cleaner, engine-specific interface.
        /// 
        /// Values correspond to array indices in mouse button state arrays,
        /// enabling efficient O(1) lookups and updates.
        /// </summary>
        enum class MouseButton : uint8_t {
            /// <summary>Left mouse button (primary button for most interactions)</summary>
            Left,
            /// <summary>Middle mouse button (often scroll wheel click)</summary>
            Middle,
            /// <summary>Right mouse button (secondary button, often context menu)</summary>
            Right
        };

    public:
        /// <summary>
        /// Default constructor creating an uninitialized input system.
        /// Initialize() must be called before the system can be used.
        /// All state arrays are zero-initialized by their default constructors.
        /// </summary>
        InputSystem() = default;

        /// <summary>
        /// Initializes the input system and captures initial input state.
        /// 
        /// This method:
        /// 1. Queries SDL for the current keyboard state and copies it to internal arrays
        /// 2. Captures the initial mouse position and button states
        /// 3. Sets both current and previous states to the same initial values
        /// 
        /// Must be called after SDL has been initialized but before any input queries.
        /// The initial state capture prevents false input events on the first frame.
        /// </summary>
        /// <returns>True if initialization succeeded, false on failure</returns>
        bool Initialize();

        /// <summary>
        /// Shuts down the input system and releases any resources.
        /// 
        /// Currently performs no operations as the input system uses only
        /// stack-allocated data structures. This method exists for consistency
        /// with other engine systems and future extensibility.
        /// 
        /// Safe to call multiple times or without prior Initialize() call.
        /// </summary>
        void Shutdown();

        /// <summary>
        /// Updates all input state for the current frame.
        /// 
        /// This method should be called once per frame, typically early in the
        /// game loop before any input queries are made. It performs the following:
        /// 
        /// 1. Saves current input states as previous frame states
        /// 2. Queries SDL for new keyboard state and copies to current state array
        /// 3. Queries SDL for mouse position and button states
        /// 4. Updates internal mouse button state flags based on SDL button masks
        /// 
        /// The update order ensures that input state remains consistent throughout
        /// the frame and that press/release detection works correctly.
        /// 
        /// Performance: This method performs fixed-cost operations regardless of
        /// how many keys/buttons are pressed, making it suitable for real-time use.
        /// </summary>
        void Update();

        // ========================================================================
        // KEYBOARD INPUT METHODS
        // ========================================================================

        /// <summary>
        /// Checks if a keyboard key is currently held down.
        /// 
        /// This method provides the current state of the specified key, returning
        /// true for as long as the key remains pressed. Use this for continuous
        /// input like movement or actions that should repeat while held.
        /// 
        /// The key parameter uses SDL scancode values, which are layout-independent
        /// and represent physical key positions rather than character output.
        /// </summary>
        /// <param name="key">SDL scancode representing the key to check</param>
        /// <returns>True if the key is currently pressed, false otherwise</returns>
        bool GetKeyDown(uint8_t key) const { return m_keyboardState[key]; }

        /// <summary>
        /// Checks if a keyboard key was held down in the previous frame.
        /// 
        /// This method returns the key state from the previous frame update,
        /// which is primarily used internally for press/release detection.
        /// It can also be useful for implementing input prediction or
        /// analyzing input patterns across frames.
        /// </summary>
        /// <param name="key">SDL scancode representing the key to check</param>
        /// <returns>True if the key was pressed in the previous frame, false otherwise</returns>
        bool GetPreviousKeyDown(uint8_t key) const { return m_prevKeyboardState[key]; }

        /// <summary>
        /// Detects if a keyboard key was just pressed this frame.
        /// 
        /// This method returns true only on the first frame when a key transitions
        /// from released to pressed state. It will return false on subsequent frames
        /// even if the key remains held down.
        /// 
        /// Perfect for:
        /// - Menu navigation and UI interactions
        /// - Single-action triggers (jump, shoot, interact)
        /// - Toggle functionality (pause, inventory)
        /// - Any action that should happen once per keypress
        /// 
        /// Implementation: Returns true when the key is down this frame but was
        /// not down in the previous frame.
        /// </summary>
        /// <param name="key">SDL scancode representing the key to check</param>
        /// <returns>True if the key was just pressed this frame, false otherwise</returns>
        bool GetKeyPressed(uint8_t key) const { return !m_prevKeyboardState[key] && m_keyboardState[key]; }

        /// <summary>
        /// Detects if a keyboard key was just released this frame.
        /// 
        /// This method returns true only on the first frame when a key transitions
        /// from pressed to released state. Useful for:
        /// - End-of-action triggers (stop charging, release bow)
        /// - Input timing analysis (how long was key held)
        /// - State cleanup when keys are released
        /// - Implementing key-up events in custom input systems
        /// 
        /// Implementation: Returns true when the key was down in the previous frame
        /// but is not down this frame.
        /// </summary>
        /// <param name="key">SDL scancode representing the key to check</param>
        /// <returns>True if the key was just released this frame, false otherwise</returns>
        bool GetKeyReleased(uint8_t key) const { return m_prevKeyboardState[key] && !m_keyboardState[key]; }

        // ========================================================================
        // MOUSE INPUT METHODS
        // ========================================================================

        /// <summary>
        /// Checks if a mouse button is currently held down.
        /// 
        /// Returns true for as long as the specified mouse button remains pressed.
        /// Use this for continuous actions like dragging, selecting, or actions
        /// that should repeat while the button is held.
        /// </summary>
        /// <param name="button">The mouse button to check (Left, Middle, or Right)</param>
        /// <returns>True if the mouse button is currently pressed, false otherwise</returns>
        bool GetMouseButtonDown(MouseButton button) const { return m_mouseButtonState[(uint8_t)button]; }

        /// <summary>
        /// Checks if a mouse button was held down in the previous frame.
        /// 
        /// Returns the mouse button state from the previous frame, primarily used
        /// for press/release detection logic. Can be useful for input analysis
        /// or implementing custom mouse gesture recognition.
        /// </summary>
        /// <param name="button">The mouse button to check (Left, Middle, or Right)</param>
        /// <returns>True if the mouse button was pressed in the previous frame, false otherwise</returns>
        bool GetPreviousMouseButtonDown(MouseButton button) const { return m_prevMouseButtonState[(uint8_t)button]; }

        /// <summary>
        /// Detects if a mouse button was just pressed this frame.
        /// 
        /// Returns true only on the first frame when a mouse button transitions
        /// from released to pressed state. Essential for:
        /// - Click detection and UI interactions
        /// - Single-shot actions (fire weapon, place object)
        /// - Menu selection and button activation
        /// - Starting drag operations or mouse gestures
        /// 
        /// Implementation: Returns true when the button is down this frame but
        /// was not down in the previous frame.
        /// </summary>
        /// <param name="button">The mouse button to check (Left, Middle, or Right)</param>
        /// <returns>True if the mouse button was just pressed this frame, false otherwise</returns>
        bool GetMouseButtonPressed(MouseButton button) const { return !m_prevMouseButtonState[(uint8_t)button] && m_mouseButtonState[(uint8_t)button]; }

        /// <summary>
        /// Detects if a mouse button was just released this frame.
        /// 
        /// Returns true only on the first frame when a mouse button transitions
        /// from pressed to released state. Useful for:
        /// - Ending drag operations
        /// - Click completion detection
        /// - Releasing held actions
        /// - Mouse gesture completion
        /// 
        /// Implementation: Returns true when the button was down in the previous
        /// frame but is not down this frame.
        /// </summary>
        /// <param name="button">The mouse button to check (Left, Middle, or Right)</param>
        /// <returns>True if the mouse button was just released this frame, false otherwise</returns>
        bool GetMouseButtonReleased(MouseButton button) const { return m_prevMouseButtonState[(uint8_t)button] && !m_mouseButtonState[(uint8_t)button]; }

        /// <summary>
        /// Gets the current mouse position in screen coordinates.
        /// 
        /// Returns the mouse cursor position relative to the window's client area,
        /// with (0,0) typically at the top-left corner. The position is updated
        /// every frame during Update() and represents the cursor location at the
        /// time of the last update.
        /// 
        /// Coordinates are in pixels and match the window's coordinate system.
        /// Use this for cursor-following elements, mouse-based camera controls,
        /// or UI element positioning.
        /// </summary>
        /// <returns>Vector2 containing the current mouse position (x, y) in pixels</returns>
        const glm::vec2& GetMousePosition() const { return m_mousePosition; }

        /// <summary>
        /// Gets the mouse position from the previous frame.
        /// 
        /// Returns the mouse cursor position from the previous frame update.
        /// This is primarily used for calculating mouse movement delta but can
        /// also be useful for implementing mouse trails, gesture recognition,
        /// or input smoothing algorithms.
        /// </summary>
        /// <returns>Vector2 containing the previous frame's mouse position (x, y) in pixels</returns>
        const glm::vec2& GetPreviousMousePosition() const { return m_prevMousePosition; }

        /// <summary>
        /// Calculates the mouse movement delta since the last frame.
        /// 
        /// Returns the difference between current and previous mouse positions,
        /// representing how far the mouse moved since the last frame. This is
        /// essential for:
        /// - Mouse look camera controls (FPS-style camera)
        /// - Relative mouse movement for UI dragging
        /// - Mouse sensitivity calculations
        /// - Smooth camera following with mouse input
        /// 
        /// The delta will be (0,0) if the mouse hasn't moved, positive values
        /// indicate movement right/down, and negative values indicate movement
        /// left/up relative to the previous frame.
        /// </summary>
        /// <returns>Vector2 containing the mouse movement delta (deltaX, deltaY) in pixels</returns>
        glm::vec2 GetMouseDelta() const { return m_mousePosition - m_prevMousePosition; }

    private:
        /// <summary>
        /// Current frame keyboard state array indexed by SDL scancode values.
        /// 
        /// Each element represents the current pressed state of a key, with true
        /// indicating pressed and false indicating released. Uses SDL_SCANCODE_COUNT
        /// as the fixed size to accommodate all possible SDL scancodes.
        /// 
        /// Updated each frame during Update() by copying from SDL's keyboard state.
        /// This provides O(1) key state lookups and ensures consistent state
        /// throughout each frame.
        /// </summary>
        std::array<bool, SDL_SCANCODE_COUNT> m_keyboardState{};

        /// <summary>
        /// Previous frame keyboard state array for press/release detection.
        /// 
        /// Contains the keyboard state from the previous frame, enabling detection
        /// of state transitions between frames. This is essential for implementing
        /// GetKeyPressed() and GetKeyReleased() functionality.
        /// 
        /// Updated at the beginning of each Update() call by copying the current
        /// state before querying new input from SDL.
        /// </summary>
        std::array<bool, SDL_SCANCODE_COUNT> m_prevKeyboardState{};

        /// <summary>
        /// Current mouse cursor position in window coordinates.
        /// 
        /// Stored as a 2D vector with x and y components representing pixel
        /// coordinates relative to the window's client area. Updated each frame
        /// during Update() by querying SDL_GetMouseState().
        /// 
        /// The position uses float values to match the vec2 type and provide
        /// compatibility with other engine systems that may perform mathematical
        /// operations on mouse coordinates.
        /// </summary>
        glm::vec2 m_mousePosition{ 0, 0 };

        /// <summary>
        /// Previous frame mouse cursor position for delta calculation.
        /// 
        /// Contains the mouse position from the previous frame, enabling calculation
        /// of mouse movement delta and providing historical position data for
        /// input analysis or smoothing algorithms.
        /// 
        /// Updated at the beginning of each Update() call before querying the
        /// new mouse position from SDL.
        /// </summary>
        glm::vec2 m_prevMousePosition{ 0, 0 };

        /// <summary>
        /// Current frame mouse button state array.
        /// 
        /// Fixed-size array with three elements corresponding to Left, Middle, and
        /// Right mouse buttons. Each element is true when the button is pressed
        /// and false when released.
        /// 
        /// Updated each frame during Update() by checking SDL mouse button masks
        /// against the SDL_GetMouseState() return value. The array indices match
        /// the MouseButton enumeration values for efficient lookups.
        /// </summary>
        std::array<bool, 3> m_mouseButtonState{ false, false, false };

        /// <summary>
        /// Previous frame mouse button state array for press/release detection.
        /// 
        /// Contains the mouse button states from the previous frame, enabling
        /// detection of button press and release events. This is essential for
        /// implementing GetMouseButtonPressed() and GetMouseButtonReleased()
        /// functionality.
        /// 
        /// Updated at the beginning of each Update() call by copying the current
        /// button states before querying new input from SDL.
        /// </summary>
        std::array<bool, 3> m_prevMouseButtonState{ false, false, false };
    };
}