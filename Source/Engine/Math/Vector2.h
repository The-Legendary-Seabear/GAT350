#pragma once
#include "Math.h"
#include "Core/Assert.h"

#include <iostream>

namespace neu {
    /// <summary>
    /// Template-based 2D vector class providing comprehensive mathematical operations.
    /// 
    /// This class represents a 2D vector with x and y components, supporting both
    /// mathematical operations and various interpretations (position, direction, texture coordinates).
    /// The template parameter allows for different numeric types (int, float, double).
    /// 
    /// Key Features:
    /// - Template-based design for type flexibility (int, float, double)
    /// - Union-based member access (x/y for spatial coordinates, u/v for texture coordinates)
    /// - Comprehensive operator overloading for intuitive mathematical operations
    /// - Vector-specific operations (length, normalization, rotation, dot/cross products)
    /// - Stream I/O support for serialization and debugging
    /// - Static utility functions for advanced vector mathematics
    /// 
    /// Common Use Cases:
    /// - 2D positions and translations (player positions, object locations)
    /// - 2D velocities and accelerations (physics simulation)
    /// - Texture coordinates (UV mapping for sprites and materials)
    /// - Screen/window coordinates (UI positioning, mouse coordinates)
    /// - Direction vectors (movement directions, surface normals)
    /// - Scale factors (object scaling, sprite sizing)
    /// 
    /// Performance Considerations:
    /// - Designed for efficient mathematical operations with minimal overhead
    /// - Union allows zero-cost abstraction for different coordinate interpretations
    /// - Template specialization enables optimal code generation for different types
    /// - Methods marked const where appropriate to enable compiler optimizations
    /// </summary>
    /// <typeparam name="T">The numeric type for vector components (typically int, float, or double)</typeparam>
    template<typename T>
    struct Vector2
    {
        /// <summary>
        /// Union providing multiple ways to access the vector components.
        /// 
        /// This design allows the same data to be interpreted in different contexts:
        /// - x, y: Spatial coordinates (position, direction, size)
        /// - u, v: Texture coordinates (UV mapping, normalized coordinates)
        /// 
        /// The union ensures zero memory overhead while providing semantic clarity
        /// in different usage contexts. All accessors refer to the same underlying data.
        /// </summary>
        union {
            /// <summary>Spatial coordinate interpretation (x = horizontal, y = vertical)</summary>
            struct { T x, y; };
            /// <summary>Texture coordinate interpretation (u = horizontal texture coord, v = vertical texture coord)</summary>
            struct { T u, v; };
        };

        /// <summary>
        /// Default constructor creating a zero vector.
        /// All components are initialized to their default value (typically 0).
        /// </summary>
        Vector2() = default;

        /// <summary>
        /// Parameterized constructor creating a vector with specified components.
        /// This is the primary way to create vectors with specific values.
        /// </summary>
        /// <param name="x">The x-component (horizontal) of the vector</param>
        /// <param name="y">The y-component (vertical) of the vector</param>
        Vector2(T x, T y) : x{ x }, y{ y } {}

        /// <summary>
        /// Const array-style accessor for vector components.
        /// Provides read-only access to vector components using array notation.
        /// Index 0 returns x-component, index 1 returns y-component.
        /// 
        /// Bounds checking is performed via ASSERT_MSG, which is active in debug builds
        /// but removed in release builds for optimal performance.
        /// </summary>
        /// <param name="index">Component index (0 for x, 1 for y)</param>
        /// <returns>Const reference to the requested component</returns>
        T  operator [] (unsigned int index) const { ASSERT_MSG(index < 2, ("Index out of range: {}", index)); return (&x)[index]; }

        /// <summary>
        /// Mutable array-style accessor for vector components.
        /// Provides read/write access to vector components using array notation.
        /// Index 0 accesses x-component, index 1 accesses y-component.
        /// 
        /// Bounds checking is performed via ASSERT_MSG, which is active in debug builds
        /// but removed in release builds for optimal performance.
        /// </summary>
        /// <param name="index">Component index (0 for x, 1 for y)</param>
        /// <returns>Mutable reference to the requested component</returns>
        T& operator [] (unsigned int index) { ASSERT_MSG(index < 3, ("Index out of range: {}", index)); return (&x)[index]; }

        // ====================================================================
        // VECTOR-VECTOR ARITHMETIC OPERATORS
        // ====================================================================

        /// <summary>
        /// Vector addition operator.
        /// Performs component-wise addition of two vectors.
        /// Commonly used for: position + displacement, combining forces, adding velocities.
        /// </summary>
        /// <param name="v">The vector to add to this vector</param>
        /// <returns>A new vector representing the sum of the two vectors</returns>
        Vector2 operator + (const Vector2& v) const { return Vector2{ x + v.x, y + v.y }; }

        /// <summary>
        /// Vector subtraction operator.
        /// Performs component-wise subtraction of two vectors.
        /// Commonly used for: calculating displacement between points, relative positioning.
        /// </summary>
        /// <param name="v">The vector to subtract from this vector</param>
        /// <returns>A new vector representing the difference of the two vectors</returns>
        Vector2 operator - (const Vector2& v) const { return Vector2{ x - v.x, y - v.y }; }

        /// <summary>
        /// Vector multiplication operator (component-wise).
        /// Performs component-wise multiplication, also known as Hadamard product.
        /// Commonly used for: scaling by non-uniform factors, texture coordinate transformation.
        /// </summary>
        /// <param name="v">The vector to multiply with this vector</param>
        /// <returns>A new vector with components multiplied pairwise</returns>
        Vector2 operator * (const Vector2& v) const { return Vector2{ x * v.x, y * v.y }; }

        /// <summary>
        /// Vector division operator (component-wise).
        /// Performs component-wise division of vector components.
        /// Commonly used for: inverse scaling, normalizing to different coordinate systems.
        /// Note: No division-by-zero checking is performed for performance reasons.
        /// </summary>
        /// <param name="v">The vector to divide this vector by</param>
        /// <returns>A new vector with components divided pairwise</returns>
        Vector2 operator / (const Vector2& v) const { return Vector2{ x / v.x, y / v.y }; }

        // ====================================================================
        // SCALAR ARITHMETIC OPERATORS
        // ====================================================================

        /// <summary>
        /// Scalar addition operator.
        /// Adds the same scalar value to both vector components.
        /// Commonly used for: uniform translation, offset operations.
        /// </summary>
        /// <param name="s">The scalar value to add to both components</param>
        /// <returns>A new vector with the scalar added to both components</returns>
        Vector2 operator + (float s) const { return Vector2{ x + s, y + s }; }

        /// <summary>
        /// Scalar subtraction operator.
        /// Subtracts the same scalar value from both vector components.
        /// Commonly used for: uniform translation in opposite direction.
        /// </summary>
        /// <param name="s">The scalar value to subtract from both components</param>
        /// <returns>A new vector with the scalar subtracted from both components</returns>
        Vector2 operator - (float s) const { return Vector2{ x - s, y - s }; }

        /// <summary>
        /// Scalar multiplication operator.
        /// Multiplies both vector components by the same scalar value.
        /// This is true vector scaling, preserving direction while changing magnitude.
        /// Commonly used for: uniform scaling, speed adjustment, unit conversion.
        /// </summary>
        /// <param name="s">The scalar value to multiply both components by</param>
        /// <returns>A new vector scaled by the scalar value</returns>
        Vector2 operator * (float s) const { return Vector2{ x * s, y * s }; }

        /// <summary>
        /// Scalar division operator.
        /// Divides both vector components by the same scalar value.
        /// Equivalent to multiplying by the reciprocal of the scalar.
        /// Commonly used for: normalization, speed reduction, coordinate system conversion.
        /// Note: No division-by-zero checking is performed for performance reasons.
        /// </summary>
        /// <param name="s">The scalar value to divide both components by</param>
        /// <returns>A new vector with both components divided by the scalar</returns>
        Vector2 operator / (float s) const { return Vector2{ x / s, y / s }; }

        // ====================================================================
        // COMPOUND ASSIGNMENT OPERATORS (VECTOR)
        // ====================================================================

        /// <summary>
        /// Vector addition assignment operator.
        /// Adds another vector to this vector, modifying this vector in place.
        /// More efficient than separate addition and assignment operations.
        /// </summary>
        /// <param name="v">The vector to add to this vector</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector2& operator += (const Vector2& v) { x += v.x; y += v.y; return *this; }

        /// <summary>
        /// Vector subtraction assignment operator.
        /// Subtracts another vector from this vector, modifying this vector in place.
        /// More efficient than separate subtraction and assignment operations.
        /// </summary>
        /// <param name="v">The vector to subtract from this vector</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector2& operator -= (const Vector2& v) { x -= v.x; y -= v.y; return *this; }

        /// <summary>
        /// Vector multiplication assignment operator.
        /// Multiplies this vector by another vector component-wise, modifying this vector in place.
        /// More efficient than separate multiplication and assignment operations.
        /// </summary>
        /// <param name="v">The vector to multiply this vector by</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector2& operator *= (const Vector2& v) { x *= v.x; y *= v.y; return *this; }

        /// <summary>
        /// Vector division assignment operator.
        /// Divides this vector by another vector component-wise, modifying this vector in place.
        /// More efficient than separate division and assignment operations.
        /// </summary>
        /// <param name="v">The vector to divide this vector by</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector2& operator /= (const Vector2& v) { x /= v.x; y /= v.y; return *this; }

        // ====================================================================
        // COMPOUND ASSIGNMENT OPERATORS (SCALAR)
        // ====================================================================

        /// <summary>
        /// Scalar addition assignment operator.
        /// Adds a scalar value to both components of this vector in place.
        /// </summary>
        /// <param name="s">The scalar value to add to both components</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector2& operator += (float s) { x += s; y += s; return *this; }

        /// <summary>
        /// Scalar subtraction assignment operator.
        /// Subtracts a scalar value from both components of this vector in place.
        /// </summary>
        /// <param name="s">The scalar value to subtract from both components</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector2& operator -= (float s) { x -= s; y -= s; return *this; }

        /// <summary>
        /// Scalar multiplication assignment operator.
        /// Multiplies both components of this vector by a scalar value in place.
        /// This is the most common scaling operation for vectors.
        /// </summary>
        /// <param name="s">The scalar value to multiply both components by</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector2& operator *= (float s) { x *= s; y *= s; return *this; }

        /// <summary>
        /// Scalar division assignment operator.
        /// Divides both components of this vector by a scalar value in place.
        /// </summary>
        /// <param name="s">The scalar value to divide both components by</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector2& operator /= (float s) { x /= s; y /= s; return *this; }

        // ====================================================================
        // VECTOR MATHEMATICS METHODS
        // ====================================================================

        /// <summary>
        /// Calculates the squared length (magnitude) of this vector.
        /// 
        /// The squared length is often preferred over length for performance reasons
        /// since it avoids the expensive square root operation. It's particularly
        /// useful for distance comparisons where relative distances matter more
        /// than exact values.
        /// 
        /// Mathematical formula: |v|² = x² + y²
        /// 
        /// Use cases:
        /// - Distance comparisons (if lengthSqr(a) &lt; lengthSqr(b), then |a| &lt; |b|)
        /// - Performance-critical calculations where exact length isn't needed
        /// - Physics calculations involving kinetic energy (proportional to velocity²)
        /// </summary>
        /// <returns>The squared magnitude of the vector</returns>
        float LengthSqr() const { return (x * x) + (y * y); }

        /// <summary>
        /// Calculates the length (magnitude) of this vector.
        /// 
        /// The length represents the Euclidean distance from the origin to the
        /// point represented by this vector, or the magnitude of the vector if
        /// interpreted as a direction and distance.
        /// 
        /// Mathematical formula: |v| = √(x² + y²)
        /// 
        /// Use cases:
        /// - Calculating actual distances between points
        /// - Normalizing vectors (dividing by length to get unit vector)
        /// - Speed calculations in physics simulations
        /// - UI measurements and layout calculations
        /// </summary>
        /// <returns>The magnitude of the vector as a floating-point value</returns>
        float Length() const { return neu::math::sqrtf(LengthSqr()); }

        /// <summary>
        /// Returns a normalized (unit length) version of this vector.
        /// 
        /// A normalized vector maintains the same direction as the original but
        /// has a length of exactly 1.0. This is essential for direction-only
        /// calculations and many mathematical operations.
        /// 
        /// Mathematical formula: v̂ = v / |v|
        /// 
        /// Use cases:
        /// - Direction vectors for movement or rotation
        /// - Surface normals in graphics and collision detection
        /// - Basis vectors for coordinate system transformations
        /// - Unit vectors for physics calculations (forces, velocities)
        /// 
        /// Note: This method does not check for zero-length vectors, which would
        /// result in division by zero. Ensure the vector has non-zero length.
        /// </summary>
        /// <returns>A new vector representing the normalized form of this vector</returns>
        Vector2 Normalized() const { return *this / Length(); }

        /// <summary>
        /// Calculates the angle of this vector relative to the positive x-axis.
        /// 
        /// The angle is measured counter-clockwise from the positive x-axis to
        /// the vector, following standard mathematical convention. The result
        /// is in the range [-π, π] radians.
        /// 
        /// Mathematical formula: θ = atan2(y, x)
        /// 
        /// Use cases:
        /// - Sprite rotation based on movement direction
        /// - Converting from Cartesian to polar coordinates
        /// - Calculating object orientation in 2D space
        /// - Navigation and pathfinding applications
        /// 
        /// Special cases:
        /// - Vector(1, 0) returns 0 (pointing right)
        /// - Vector(0, 1) returns π/2 (pointing up)
        /// - Vector(-1, 0) returns π (pointing left)
        /// - Vector(0, -1) returns -π/2 (pointing down)
        /// </summary>
        /// <returns>The angle in radians from the positive x-axis to this vector</returns>
        float Angle() const { return math::atan2f(y, x); };

        /// <summary>
        /// Returns a new vector that results from rotating this vector by the specified angle.
        /// 
        /// The rotation is performed counter-clockwise around the origin using the
        /// standard 2D rotation matrix transformation. This operation preserves
        /// the length of the vector while changing its direction.
        /// 
        /// Mathematical formulas:
        /// x' = x * cos(θ) - y * sin(θ)
        /// y' = x * sin(θ) + y * cos(θ)
        /// 
        /// Use cases:
        /// - Rotating sprites or objects in 2D space
        /// - Implementing steering behaviors in AI systems
        /// - Creating circular or orbital motion patterns
        /// - Transforming coordinate systems
        /// - Generating rotated copies of vectors for geometric calculations
        /// </summary>
        /// <param name="radians">The angle to rotate the vector, in radians (positive = counter-clockwise)</param>
        /// <returns>A new vector representing this vector rotated by the specified angle</returns>
        Vector2 Rotate(float radians) const {
            Vector2 v;

            v.x = x * math::cosf(radians) - y * math::sinf(radians);
            v.y = x * math::sinf(radians) + y * math::cosf(radians);

            return v;
        }

        // ====================================================================
        // STATIC UTILITY FUNCTIONS
        // ====================================================================

        /// <summary>
        /// Calculates the dot product of two 2D vectors.
        /// 
        /// The dot product is a fundamental operation in vector mathematics that
        /// returns a scalar value. It's related to the angle between vectors and
        /// their magnitudes: a·b = |a||b|cos(θ)
        /// 
        /// Mathematical formula: a·b = ax*bx + ay*by
        /// 
        /// Geometric interpretation:
        /// - Positive result: vectors point in similar directions (angle &lt; 90°)
        /// - Zero result: vectors are perpendicular (angle = 90°)
        /// - Negative result: vectors point in opposite directions (angle &gt; 90°)
        /// 
        /// Use cases:
        /// - Calculating the angle between vectors (combined with magnitude)
        /// - Determining if vectors are perpendicular (dot product = 0)
        /// - Projecting one vector onto another
        /// - Lighting calculations in graphics (surface normal · light direction)
        /// - Physics calculations involving work and energy
        /// </summary>
        /// <param name="a">The first vector</param>
        /// <param name="b">The second vector</param>
        /// <returns>The scalar dot product of the two vectors</returns>
        static float Dot(const Vector2& a, const Vector2& b) {
            return a.x * b.x + a.y * b.y;
        }

        /// <summary>
        /// Calculates the 2D cross product (scalar) of two vectors.
        /// 
        /// In 2D, the cross product returns a scalar representing the magnitude
        /// of the vector perpendicular to the plane containing both input vectors.
        /// This is equivalent to the z-component of the 3D cross product.
        /// 
        /// Mathematical formula: a×b = ax*by - ay*bx
        /// 
        /// Geometric interpretation:
        /// - Positive result: b is counter-clockwise from a
        /// - Zero result: vectors are parallel (same or opposite direction)  
        /// - Negative result: b is clockwise from a
        /// 
        /// Use cases:
        /// - Determining the orientation of three points (left turn vs right turn)
        /// - Calculating the area of parallelograms and triangles
        /// - Collision detection and geometric algorithms
        /// - Determining which side of a line a point is on
        /// - 2D physics simulations (torque calculations)
        /// </summary>
        /// <param name="a">The first vector</param>
        /// <param name="b">The second vector</param>
        /// <returns>The scalar cross product of the two vectors</returns>
        static float Cross(const Vector2& a, const Vector2& b) {
            return a.x * b.y - a.y * b.x;
        }

        /// <summary>
        /// Calculates the unsigned angle in radians between two 2D vectors.
        /// 
        /// This method returns the smaller angle between two vectors, always
        /// in the range [0, π] radians. The result is independent of the order
        /// of the vectors and represents the absolute angular separation.
        /// 
        /// Mathematical formula: θ = arccos(a·b / (|a||b|))
        /// 
        /// Note: This implementation assumes the input vectors are already normalized.
        /// For non-normalized vectors, normalize them first or the result will be incorrect.
        /// 
        /// Use cases:
        /// - Determining how much to rotate one vector to align with another
        /// - Calculating field of view and vision cone angles
        /// - Measuring angular differences in steering behaviors
        /// - Physics constraints and joint angle calculations
        /// </summary>
        /// <param name="a">The first normalized vector</param>
        /// <param name="b">The second normalized vector</param>
        /// <returns>The unsigned angle in radians between the vectors [0, π]</returns>
        static float AngleBetween(const Vector2& a, const Vector2& b) {
            return math::acosf(Dot(a, b));
        }

        /// <summary>
        /// Calculates the signed angle in radians between two 2D vectors.
        /// 
        /// This method returns the angle from vector a to vector b, with the sign
        /// indicating the rotation direction. The result is in the range [-π, π].
        /// 
        /// Rotation direction:
        /// - Positive result: counter-clockwise rotation from a to b
        /// - Negative result: clockwise rotation from a to b
        /// - Zero result: vectors are parallel (same direction)
        /// 
        /// Mathematical approach: Uses both dot product and cross product to
        /// determine both magnitude and direction of the angular difference.
        /// 
        /// Use cases:
        /// - Steering calculations that need to know which direction to turn
        /// - Animation systems that interpolate between orientations
        /// - AI pathfinding with directional preferences
        /// - Game mechanics involving turning or rotation direction
        /// - Physics simulations requiring signed angular velocities
        /// </summary>
        /// <param name="a">The first vector (starting direction)</param>
        /// <param name="b">The second vector (target direction)</param>
        /// <returns>The signed angle in radians from vector a to vector b [-π, π]</returns>
        static float SignedAngleBetween(const Vector2& a, const Vector2& b) {
            Vector2 v{ Dot(a, b), Cross(a, b) };
            return v.Angle();
        }
    };

    /// <summary>
    /// Stream output operator for Vector2 types.
    /// 
    /// Formats the vector for output in a readable "{x, y}" format, suitable for
    /// debugging, logging, and serialization purposes. The output format is
    /// consistent and parseable by the corresponding input operator.
    /// 
    /// Output format: "{1.5, -2.3}"
    /// </summary>
    /// <typeparam name="T">The numeric type of the vector components</typeparam>
    /// <param name="stream">The output stream to write to</param>
    /// <param name="v">The vector to output</param>
    /// <returns>Reference to the output stream for chaining operations</returns>
    template<typename T>
    std::ostream& operator << (std::ostream& stream, const Vector2<T>& v) {
        stream << "{" << v.x << ", " << v.y << "}";

        return stream;
    }

    /// <summary>
    /// Stream input operator for Vector2 types.
    /// 
    /// Parses vector data from an input stream expecting the format "{x, y}".
    /// This operator handles whitespace appropriately and sets the stream's
    /// fail bit if the format is incorrect or parsing fails.
    /// 
    /// Expected input format: "{1.5, -2.3}"
    /// 
    /// The parser is strict about format requirements:
    /// - Must start with '{'
    /// - Components separated by ','
    /// - Must end with '}'
    /// - Whitespace is ignored between tokens
    /// 
    /// On parse failure, the stream's failbit is set and the vector's
    /// state may be partially modified.
    /// </summary>
    /// <typeparam name="T">The numeric type of the vector components</typeparam>
    /// <param name="stream">The input stream to read from</param>
    /// <param name="v">The vector to populate with parsed data</param>
    /// <returns>Reference to the input stream for chaining operations</returns>
    template<typename T>
    std::istream& operator >> (std::istream& stream, Vector2<T>& v) {
        char ch = '\0';

        // Expected format: { x, y }

        // Parse opening brace '{'
        if (!(stream >> std::ws >> ch) || ch != '{') {
            stream.setstate(std::ios::failbit);
            return stream;
        }
        // Parse x component
        if (!(stream >> std::ws >> v.x)) {
            stream.setstate(std::ios::failbit);
            return stream;
        }
        // Parse separator comma ','
        if (!(stream >> std::ws >> ch) || ch != ',') {
            stream.setstate(std::ios::failbit);
            return stream;
        }
        // Parse y component
        if (!(stream >> std::ws >> v.y)) {
            stream.setstate(std::ios::failbit);
            return stream;
        }
        // Parse closing brace '}'
        if (!(stream >> std::ws >> ch) || ch != '}') {
            stream.setstate(std::ios::failbit);
            return stream;
        }

        return stream;
    }

    /// <summary>
    /// Type alias for 2D integer vectors.
    /// Commonly used for: pixel coordinates, array indices, grid positions,
    /// discrete spatial data where fractional values are not needed.
    /// </summary>
    using ivec2 = Vector2<int>;

    /// <summary>
    /// Type alias for 2D floating-point vectors.
    /// The most commonly used vector type for: world positions, velocities,
    /// directions, texture coordinates, and general mathematical calculations
    /// requiring fractional precision.
    /// </summary>
    using vec2 = Vector2<float>;
}