namespace neu {
    /// <summary>
    /// A generic rectangle template structure representing a 2D rectangular area.
    /// The rectangle is defined by its position (x, y) and dimensions (width, height).
    /// This template can be instantiated with any numeric type (int, float, double, etc.).
    /// </summary>
    /// <typeparam name="T">The numeric type for position and size values (e.g., int, float, double)</typeparam>
    template <typename T>
    struct Rect {
        /// <summary>X coordinate of the rectangle's top-left corner</summary>
        T x;

        /// <summary>Y coordinate of the rectangle's top-left corner</summary>
        T y;

        /// <summary>Width of the rectangle</summary>
        T w;

        /// <summary>Height of the rectangle</summary>
        T h;

        /// <summary>
        /// Default constructor that creates a rectangle at origin (0,0) with zero dimensions.
        /// Initializes all values (x, y, w, h) to zero using the default value of type T.
        /// </summary>
        Rect() : x{ 0 }, y{ 0 }, w{ 0 }, h{ 0 } {}

        /// <summary>
        /// Parameterized constructor that creates a rectangle with specified position and dimensions.
        /// </summary>
        /// <param name="x">X coordinate of the top-left corner</param>
        /// <param name="y">Y coordinate of the top-left corner</param>
        /// <param name="w">Width of the rectangle (should be positive)</param>
        /// <param name="h">Height of the rectangle (should be positive)</param>
        Rect(T x, T y, T w, T h) : x{ x }, y{ y }, w{ w }, h{ h } {}
    };

    /// <summary>
    /// Type alias for a floating-point rectangle.
    /// Commonly used for graphics rendering, UI layouts, and situations requiring sub-pixel precision.
    /// </summary>
    using rect = Rect<float>;

    /// <summary>
    /// Type alias for an integer rectangle.
    /// Commonly used for pixel-perfect graphics, grid-based systems, and screen coordinates.
    /// </summary>
    using irect = Rect<int>;
}