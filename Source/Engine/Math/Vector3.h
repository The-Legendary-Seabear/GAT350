#pragma once
#include "Math.h"
#include "Core/Assert.h"

#include <iostream>

namespace neu {
    /// <summary>
    /// Template-based 3D vector class providing comprehensive mathematical operations for three-dimensional calculations.
    /// 
    /// This class represents a 3D vector with x, y, and z components, supporting both
    /// mathematical operations and various interpretations (position, direction, color values).
    /// The template parameter allows for different numeric types (int, float, double).
    /// 
    /// Key Features:
    /// - Template-based design for type flexibility (int, float, double)
    /// - Union-based member access (x/y/z for spatial coordinates, r/g/b for color values)
    /// - Comprehensive operator overloading for intuitive mathematical operations
    /// - Vector-specific operations (length calculations, magnitude operations)
    /// - Stream I/O support for serialization and debugging
    /// - Bounds-checked array access with assertion-based error detection
    /// 
    /// Common Use Cases:
    /// - 3D positions and translations (world coordinates, object locations)
    /// - 3D velocities and accelerations (physics simulation, movement)
    /// - RGB color values (rendering, lighting, material properties)
    /// - Direction vectors (movement directions, surface normals, lighting vectors)
    /// - Scale factors (3D object scaling, transformation matrices)
    /// - Euler angles (rotation representation in 3D space)
    /// - Vertex positions (3D geometry, mesh data)
    /// 
    /// Performance Considerations:
    /// - Designed for efficient mathematical operations with minimal overhead
    /// - Union allows zero-cost abstraction for different coordinate interpretations
    /// - Template specialization enables optimal code generation for different types
    /// - Methods marked const where appropriate to enable compiler optimizations
    /// - Assertion-based bounds checking (removed in release builds for performance)
    /// </summary>
    /// <typeparam name="T">The numeric type for vector components (typically int, float, or double)</typeparam>
    template<typename T>
    struct Vector3
    {
        /// <summary>
        /// Union providing multiple ways to access the vector components.
        /// 
        /// This design allows the same data to be interpreted in different contexts:
        /// - x, y, z: 3D spatial coordinates (position, direction, scale)
        /// - r, g, b: RGB color components (red, green, blue values)
        /// 
        /// The union ensures zero memory overhead while providing semantic clarity
        /// in different usage contexts. All accessors refer to the same underlying data.
        /// 
        /// Usage examples:
        /// - vec3 position{10.0f, 5.0f, -3.0f}; // Spatial coordinates
        /// - vec3 color{1.0f, 0.5f, 0.2f}; color.r = 0.8f; // Color manipulation
        /// </summary>
        union {
            /// <summary>Spatial coordinate interpretation (x = horizontal, y = vertical, z = depth)</summary>
            struct { T x, y, z; };
            /// <summary>Color component interpretation (r = red, g = green, b = blue)</summary>
            struct { T r, g, b; };
        };

        /// <summary>
        /// Default constructor creating a zero vector.
        /// All components are initialized to their default value (typically 0).
        /// Useful for: origin points, default initialization, placeholder vectors.
        /// </summary>
        Vector3() = default;

        /// <summary>
        /// Parameterized constructor creating a vector with specified components.
        /// This is the primary way to create vectors with specific values.
        /// </summary>
        /// <param name="x">The x-component (horizontal/red) of the vector</param>
        /// <param name="y">The y-component (vertical/green) of the vector</param>
        /// <param name="z">The z-component (depth/blue) of the vector</param>
        Vector3(T x, T y, T z) : x{ x }, y{ y }, z{ z } {}

        /// <summary>
        /// Const array-style accessor for vector components with bounds checking.
        /// Provides read-only access to vector components using array notation.
        /// Index 0 returns x, index 1 returns y, index 2 returns z.
        /// 
        /// Bounds checking is performed via ASSERT_MSG, which is active in debug builds
        /// but removed in release builds for optimal performance.
        /// </summary>
        /// <param name="index">Component index (0 for x, 1 for y, 2 for z)</param>
        /// <returns>Const reference to the requested component</returns>
        T  operator [] (unsigned int index) const { ASSERT_MSG(index < 3, ("Index out of range: {}", index)); return (&x)[index]; }

        /// <summary>
        /// Mutable array-style accessor for vector components with bounds checking.
        /// Provides read/write access to vector components using array notation.
        /// Index 0 accesses x, index 1 accesses y, index 2 accesses z.
        /// 
        /// Bounds checking is performed via ASSERT_MSG, which is active in debug builds
        /// but removed in release builds for optimal performance.
        /// </summary>
        /// <param name="index">Component index (0 for x, 1 for y, 2 for z)</param>
        /// <returns>Mutable reference to the requested component</returns>
        T& operator [] (unsigned int index) { ASSERT_MSG(index < 3, ("Index out of range: {}", index)); return (&x)[index]; }

        // ====================================================================
        // VECTOR-VECTOR ARITHMETIC OPERATORS
        // ====================================================================

        /// <summary>
        /// Vector addition operator.
        /// Performs component-wise addition of two 3D vectors.
        /// 
        /// Mathematical formula: (a.x + b.x, a.y + b.y, a.z + b.z)
        /// 
        /// Common uses:
        /// - Position + displacement = new position
        /// - Combining multiple forces in physics
        /// - Adding velocities in relative motion
        /// - Color blending operations
        /// </summary>
        /// <param name="v">The vector to add to this vector</param>
        /// <returns>A new vector representing the sum of the two vectors</returns>
        Vector3 operator + (const Vector3& v) const { return Vector3{ x + v.x, y + v.y, z + v.z }; }

        /// <summary>
        /// Vector subtraction operator.
        /// Performs component-wise subtraction of two 3D vectors.
        /// 
        /// Mathematical formula: (a.x - b.x, a.y - b.y, a.z - b.z)
        /// 
        /// Common uses:
        /// - Calculating displacement between two positions
        /// - Finding relative positions (target - origin)
        /// - Vector difference calculations
        /// - Color subtraction for effects
        /// </summary>
        /// <param name="v">The vector to subtract from this vector</param>
        /// <returns>A new vector representing the difference of the two vectors</returns>
        Vector3 operator - (const Vector3& v) const { return Vector3{ x - v.x, y - v.y, z - v.z }; }

        /// <summary>
        /// Vector multiplication operator (component-wise).
        /// Performs component-wise multiplication, also known as Hadamard product.
        /// This is NOT the dot product or cross product.
        /// 
        /// Mathematical formula: (a.x * b.x, a.y * b.y, a.z * b.z)
        /// 
        /// Common uses:
        /// - Non-uniform scaling (different scale factors per axis)
        /// - Color modulation (lighting effects, tinting)
        /// - Texture coordinate transformation
        /// - Element-wise mathematical operations
        /// </summary>
        /// <param name="v">The vector to multiply with this vector</param>
        /// <returns>A new vector with components multiplied pairwise</returns>
        Vector3 operator * (const Vector3& v) const { return Vector3{ x * v.x, y * v.y, z * v.z }; }

        /// <summary>
        /// Vector division operator (component-wise).
        /// Performs component-wise division of vector components.
        /// 
        /// Mathematical formula: (a.x / b.x, a.y / b.y, a.z / b.z)
        /// 
        /// Common uses:
        /// - Inverse scaling operations
        /// - Normalizing to different coordinate systems
        /// - Converting between units or scales
        /// 
        /// Warning: No division-by-zero checking is performed for performance reasons.
        /// Ensure divisor components are non-zero to avoid undefined behavior.
        /// </summary>
        /// <param name="v">The vector to divide this vector by</param>
        /// <returns>A new vector with components divided pairwise</returns>
        Vector3 operator / (const Vector3& v) const { return Vector3{ x / v.x, y / v.y, z / v.z }; }

        // ====================================================================
        // SCALAR ARITHMETIC OPERATORS
        // ====================================================================

        /// <summary>
        /// Scalar addition operator.
        /// Adds the same scalar value to all three vector components.
        /// 
        /// Mathematical formula: (x + s, y + s, z + s)
        /// 
        /// Common uses:
        /// - Uniform translation in all directions
        /// - Brightness adjustment for colors
        /// - Offset operations
        /// </summary>
        /// <param name="s">The scalar value to add to all components</param>
        /// <returns>A new vector with the scalar added to all components</returns>
        Vector3 operator + (float s) const { return Vector3{ x + s, y + s, z + s }; }

        /// <summary>
        /// Scalar subtraction operator.
        /// Subtracts the same scalar value from all three vector components.
        /// 
        /// Mathematical formula: (x - s, y - s, z - s)
        /// 
        /// Common uses:
        /// - Uniform translation in opposite direction
        /// - Darkness adjustment for colors
        /// - Negative offset operations
        /// </summary>
        /// <param name="s">The scalar value to subtract from all components</param>
        /// <returns>A new vector with the scalar subtracted from all components</returns>
        Vector3 operator - (float s) const { return Vector3{ x - s, y - s, z - s }; }

        /// <summary>
        /// Scalar multiplication operator.
        /// Multiplies all three vector components by the same scalar value.
        /// This is true vector scaling, preserving direction while changing magnitude.
        /// 
        /// Mathematical formula: (x * s, y * s, z * s)
        /// 
        /// Common uses:
        /// - Uniform scaling in all directions
        /// - Speed/magnitude adjustment for velocities
        /// - Color intensity scaling
        /// - Unit conversion (meters to centimeters, etc.)
        /// </summary>
        /// <param name="s">The scalar value to multiply all components by</param>
        /// <returns>A new vector scaled by the scalar value</returns>
        Vector3 operator * (float s) const { return Vector3{ x * s, y * s, z * s }; }

        /// <summary>
        /// Scalar division operator.
        /// Divides all three vector components by the same scalar value.
        /// Equivalent to multiplying by the reciprocal of the scalar.
        /// 
        /// Mathematical formula: (x / s, y / s, z / s)
        /// 
        /// Common uses:
        /// - Uniform scaling down
        /// - Normalization (dividing by length)
        /// - Average calculations
        /// - Unit conversion in reverse direction
        /// 
        /// Warning: No division-by-zero checking is performed for performance reasons.
        /// Ensure the scalar is non-zero to avoid undefined behavior.
        /// </summary>
        /// <param name="s">The scalar value to divide all components by</param>
        /// <returns>A new vector with all components divided by the scalar</returns>
        Vector3 operator / (float s) const { return Vector3{ x / s, y / s, z / s }; }

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
        Vector3& operator += (const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }

        /// <summary>
        /// Vector subtraction assignment operator.
        /// Subtracts another vector from this vector, modifying this vector in place.
        /// More efficient than separate subtraction and assignment operations.
        /// </summary>
        /// <param name="v">The vector to subtract from this vector</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector3& operator -= (const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

        /// <summary>
        /// Vector multiplication assignment operator.
        /// Multiplies this vector by another vector component-wise, modifying this vector in place.
        /// More efficient than separate multiplication and assignment operations.
        /// </summary>
        /// <param name="v">The vector to multiply this vector by</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector3& operator *= (const Vector3& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }

        /// <summary>
        /// Vector division assignment operator.
        /// Divides this vector by another vector component-wise, modifying this vector in place.
        /// More efficient than separate division and assignment operations.
        /// </summary>
        /// <param name="v">The vector to divide this vector by</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector3& operator /= (const Vector3& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

        // ====================================================================
        // COMPOUND ASSIGNMENT OPERATORS (SCALAR)
        // ====================================================================

        /// <summary>
        /// Scalar addition assignment operator.
        /// Adds a scalar value to all components of this vector in place.
        /// </summary>
        /// <param name="s">The scalar value to add to all components</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector3& operator += (float s) { x += s; y += s; z += s; return *this; }

        /// <summary>
        /// Scalar subtraction assignment operator.
        /// Subtracts a scalar value from all components of this vector in place.
        /// </summary>
        /// <param name="s">The scalar value to subtract from all components</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector3& operator -= (float s) { x -= s; y -= s; z -= s; return *this; }

        /// <summary>
        /// Scalar multiplication assignment operator.
        /// Multiplies all components of this vector by a scalar value in place.
        /// This is the most common scaling operation for 3D vectors.
        /// </summary>
        /// <param name="s">The scalar value to multiply all components by</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector3& operator *= (float s) { x *= s; y *= s; z *= s; return *this; }

        /// <summary>
        /// Scalar division assignment operator.
        /// Divides all components of this vector by a scalar value in place.
        /// </summary>
        /// <param name="s">The scalar value to divide all components by</param>
        /// <returns>Reference to this vector after the operation</returns>
        Vector3& operator /= (float s) { x /= s; y /= s; z /= s; return *this; }

        // ====================================================================
        // VECTOR MATHEMATICS METHODS
        // ====================================================================

        /// <summary>
        /// Calculates the squared length (magnitude) of this 3D vector.
        /// 
        /// The squared length is often preferred over length for performance reasons
        /// since it avoids the expensive square root operation. It's particularly
        /// useful for distance comparisons where relative distances matter more
        /// than exact values.
        /// 
        /// Mathematical formula: |v|² = x² + y² + z²
        /// 
        /// Use cases:
        /// - 3D distance comparisons (if lengthSqr(a) &lt; lengthSqr(b), then |a| &lt; |b|)
        /// - Performance-critical calculations where exact length isn't needed
        /// - Physics calculations involving kinetic energy (proportional to velocity²)
        /// - Collision detection with spherical bounds (radius² comparisons)
        /// - Sorting objects by distance without expensive square root calculations
        /// </summary>
        /// <returns>The squared magnitude of the 3D vector</returns>
        float LengthSqr() const { return (x * x) + (y * y) + (z * z); }

        /// <summary>
        /// Calculates the length (magnitude) of this 3D vector.
        /// 
        /// The length represents the Euclidean distance from the origin to the
        /// point represented by this vector, or the magnitude of the vector if
        /// interpreted as a direction and distance in 3D space.
        /// 
        /// Mathematical formula: |v| = √(x² + y² + z²)
        /// 
        /// Use cases:
        /// - Calculating actual distances between 3D points
        /// - Normalizing vectors (dividing by length to get unit vector)
        /// - Speed calculations in 3D physics simulations
        /// - 3D measurements and spatial calculations
        /// - Light attenuation calculations (distance-based falloff)
        /// - Camera distance calculations for LOD (Level of Detail) systems
        /// </summary>
        /// <returns>The magnitude of the 3D vector as a floating-point value</returns>
        float Length() const { return neu::math::sqrtf(LengthSqr()); }
    };

    /// <summary>
    /// Stream output operator for Vector3 types.
    /// 
    /// Formats the 3D vector for output in a readable "{x, y, z}" format, suitable for
    /// debugging, logging, and serialization purposes. The output format is
    /// consistent and parseable by the corresponding input operator.
    /// 
    /// Output format: "{1.5, -2.3, 0.7}"
    /// 
    /// Common uses:
    /// - Debug output for 3D positions and vectors
    /// - Logging coordinate values
    /// - Configuration file output
    /// - Error reporting with spatial data
    /// </summary>
    /// <typeparam name="T">The numeric type of the vector components</typeparam>
    /// <param name="stream">The output stream to write to</param>
    /// <param name="v">The 3D vector to output</param>
    /// <returns>Reference to the output stream for chaining operations</returns>
    template<typename T>
    std::ostream& operator << (std::ostream& stream, const Vector3<T>& v) {
        stream << "{" << v.x << ", " << v.y << ", " << v.z << " }";

        return stream;
    }

    /// <summary>
    /// Stream input operator for Vector3 types.
    /// 
    /// Parses 3D vector data from an input stream expecting the format "{x, y, z}".
    /// This operator handles whitespace appropriately and sets the stream's
    /// fail bit if the format is incorrect or parsing fails.
    /// 
    /// Expected input format: "{1.5, -2.3, 0.7}"
    /// 
    /// The parser is strict about format requirements:
    /// - Must start with '{'
    /// - Three components separated by commas ','
    /// - Must end with '}'
    /// - Whitespace is ignored between tokens
    /// 
    /// On parse failure, the stream's failbit is set and the vector's
    /// state may be partially modified.
    /// 
    /// Common uses:
    /// - Reading 3D coordinates from configuration files
    /// - Parsing user input for 3D positions
    /// - Loading spatial data from text files
    /// - Deserializing saved 3D vector data
    /// </summary>
    /// <typeparam name="T">The numeric type of the vector components</typeparam>
    /// <param name="stream">The input stream to read from</param>
    /// <param name="v">The 3D vector to populate with parsed data</param>
    /// <returns>Reference to the input stream for chaining operations</returns>
    template<typename T>
    std::istream& operator >> (std::istream& stream, Vector3<T>& v) {
        char ch = '\0';

        // Expected format: { x, y, z }

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
        // Parse first separator comma ','
        if (!(stream >> std::ws >> ch) || ch != ',') {
            stream.setstate(std::ios::failbit);
            return stream;
        }
        // Parse y component
        if (!(stream >> std::ws >> v.y)) {
            stream.setstate(std::ios::failbit);
            return stream;
        }
        // Parse second separator comma ','
        if (!(stream >> std::ws >> ch) || ch != ',') {
            stream.setstate(std::ios::failbit);
            return stream;
        }
        // Parse z component
        if (!(stream >> std::ws >> v.z)) {
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
    /// Type alias for 3D integer vectors.
    /// Commonly used for: voxel coordinates, array indices, discrete 3D grid positions,
    /// RGB color values with integer components, and spatial data where fractional
    /// values are not needed.
    /// </summary>
    using ivec3 = Vector3<int>;

    /// <summary>
    /// Type alias for 3D floating-point vectors.
    /// The most commonly used 3D vector type for: world positions, 3D velocities,
    /// direction vectors, surface normals, RGB color values with floating precision,
    /// and general mathematical calculations requiring fractional precision in 3D space.
    /// </summary>
    using vec3 = Vector3<float>;
}
