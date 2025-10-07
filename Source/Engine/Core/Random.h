#pragma once
#include "Math/Vector2.h"
#include "Math/Math.h"

#include <cstdlib>
#include <random>

/// <summary>
/// Random number generation utilities namespace providing convenient functions
/// for generating various types of random values using modern C++ random facilities.
/// All functions use a shared Mersenne Twister generator for consistency and quality.
/// </summary>
namespace neu::random {
    /// <summary>
    /// Returns a reference to the shared Mersenne Twister random number generator.
    /// The generator is initialized once using a hardware random device for seeding.
    /// This provides high-quality random numbers with a long period suitable for most applications.
    /// </summary>
    /// <returns>Reference to the static mt19937 generator instance</returns>
    inline std::mt19937& generator() {
        // Hardware-based random device for seeding (when available)
        static std::random_device rd;
        // Mersenne Twister generator, seeded once on first access
        static std::mt19937 gen(rd());
        return gen;
    }

    /// <summary>
    /// Seeds the random number generator with a specific value.
    /// Useful for reproducible random sequences in testing, debugging, or deterministic simulations.
    /// </summary>
    /// <param name="value">The seed value to initialize the generator with</param>
    inline void seed(unsigned int value) {
        generator().seed(value);
    }

    /// <summary>
    /// Generates a random integer within the specified inclusive range [min, max].
    /// Both min and max values are included in the possible results.
    /// </summary>
    /// <param name="min">The minimum value (inclusive)</param>
    /// <param name="max">The maximum value (inclusive)</param>
    /// <returns>A random integer between min and max (both inclusive)</returns>
    inline int getInt(int min, int max) {
        // Uniform distribution ensures each value in range has equal probability
        std::uniform_int_distribution<> dist(min, max);
        return dist(generator());
    }

    /// <summary>
    /// Generates a random integer in the range [0, max-1].
    /// Convenient for array indexing and similar use cases.
    /// </summary>
    /// <param name="max">The upper bound (exclusive) - result will be less than this value</param>
    /// <returns>A random integer between 0 (inclusive) and max (exclusive)</returns>
    inline int getInt(int max) {
        return getInt(0, max - 1);
    }

    /// <summary>
    /// Generates a random integer using the full range of the distribution.
    /// Returns values across the entire range supported by the uniform_int_distribution.
    /// </summary>
    /// <returns>A random integer from the full distribution range</returns>
    inline int getInt() {
        // Static distribution to avoid recreation overhead
        static std::uniform_int_distribution<> dist;
        return dist(generator());
    }

    /// <summary>
    /// Generates a random real number within the specified range [min, max).
    /// The minimum value is inclusive, the maximum value is exclusive.
    /// </summary>
    /// <typeparam name="T">The floating-point type (float, double, long double). Defaults to float.</typeparam>
    /// <param name="min">The minimum value (inclusive)</param>
    /// <param name="max">The maximum value (exclusive)</param>
    /// <returns>A random real number of type T in the range [min, max)</returns>
    template <typename T = float>
    inline T getReal(T min, T max) {
        // Uniform real distribution for floating-point values
        std::uniform_real_distribution<T> dist(min, max);
        return dist(generator());
    }

    /// <summary>
    /// Generates a random real number in the range [0, max).
    /// Convenient overload for generating values from zero to a specified maximum.
    /// </summary>
    /// <typeparam name="T">The floating-point type. Defaults to float.</typeparam>
    /// <param name="max">The maximum value (exclusive)</param>
    /// <returns>A random real number of type T in the range [0, max)</returns>
    template <typename T = float>
    inline T getReal(T max) {
        return getReal(static_cast<T>(0), static_cast<T>(max));
    }

    /// <summary>
    /// Generates a random real number in the range [0, 1).
    /// This is the standard unit interval commonly used for probability calculations,
    /// normalized values, and as input to other random functions.
    /// </summary>
    /// <typeparam name="T">The floating-point type of the generated number. Defaults to float.</typeparam>
    /// <returns>A random real number of type T in the range [0, 1).</returns>
    template <typename T = float>
    inline T getReal() {
        // Static distribution to avoid recreation overhead
        static std::uniform_real_distribution<T> dist(static_cast<T>(0), static_cast<T>(1));
        return dist(generator());
    }

    /// <summary>
    /// Generates a random boolean value with 50% probability for true/false.
    /// Uses Bernoulli distribution for proper statistical properties.
    /// </summary>
    /// <returns>A random boolean value (true or false with equal probability)</returns>
    inline bool getBool() {
        // Bernoulli distribution with p=0.5 for fair coin flip
        static std::bernoulli_distribution dist(0.5);
        return dist(generator());
    }

    /// <summary>
    /// Generates a 2D vector representing a point on the unit circle.
    /// The returned vector has magnitude 1.0 and points in a random direction.
    /// Useful for generating random directions, unit vectors, or points on a circle.
    /// </summary>
    /// <returns>A vec2 containing the x and y coordinates of a point on the unit circle</returns>
    inline vec2 onUnitCircle() {
        // Generate random angle in range [0, 2π)
        float radians = getReal(math::twoPi);

        // Convert polar coordinates to Cartesian coordinates
        // cos(θ) gives x-component, sin(θ) gives y-component
        return vec2{ math::cosf(radians), math::sinf(radians) };
    }
}