#pragma once
#include "Logger.h"

/// <summary>
/// Debug assertion macros for runtime condition checking and debugging.
/// 
/// These macros provide a way to validate assumptions during development and catch
/// programming errors early. Assertions are only active in debug builds and are
/// completely removed in release builds for optimal performance.
/// 
/// Key features:
/// - Debug-only execution (no performance impact in release)
/// - Detailed error logging with condition and optional message
/// - Immediate program termination on failure
/// - Source code condition display using stringification
/// 
/// Usage guidelines:
/// - Use for checking preconditions, postconditions, and invariants
/// - Don't use for user input validation or recoverable errors
/// - Conditions should have no side effects (since they're removed in release)
/// </summary>

#ifdef _DEBUG
    /// <summary>
    /// Debug assertion macro that checks a condition and terminates if false.
    /// 
    /// In debug builds, this macro:
    /// 1. Evaluates the given condition
    /// 2. If false, logs an error with the condition text
    /// 3. Immediately terminates the program with std::abort()
    /// 
    /// The do-while(0) construct ensures the macro behaves like a single statement
    /// and works correctly with if-else statements without braces.
    /// 
    /// Example usage:
    ///   ASSERT(ptr != nullptr);
    ///   ASSERT(index < array.size());
    ///   ASSERT(velocity.magnitude() <= maxSpeed);
    /// </summary>
    /// <param name="condition">The condition to check - should be an expression that evaluates to bool</param>
#define ASSERT(condition) \
        do { \
            if (!(condition)) { \
                LOG_ERROR("ASSERTION FAILED: '{}'", #condition); \
                std::abort(); \
            } \
        } while(0)

    /// <summary>
    /// Debug assertion macro with custom error message.
    /// 
    /// Similar to ASSERT() but allows providing additional context through a custom message.
    /// This is particularly useful for complex conditions or when you want to provide
    /// guidance about what went wrong or how to fix it.
    /// 
    /// The macro logs both the failed condition and the custom message, making
    /// debugging easier by providing more context about the failure.
    /// 
    /// Example usage:
    ///   ASSERT_MSG(file.is_open(), "Failed to open configuration file");
    ///   ASSERT_MSG(player_count > 0, "Game started with no players");
    ///   ASSERT_MSG(texture_width > 0, "Texture must have positive dimensions");
    /// </summary>
    /// <param name="condition">The condition to check - should be an expression that evaluates to bool</param>
    /// <param name="msg">Custom error message providing additional context about the failure</param>
#define ASSERT_MSG(condition, msg) \
        do { \
            if (!(condition)) { \
                LOG_ERROR("ASSERTION FAILED: '{}' - {}", #condition, msg); \
                std::abort(); \
            } \
        } while(0)

#else
    /// <summary>
    /// Release build version of ASSERT - completely removed for performance.
    /// 
    /// In release builds, this macro expands to ((void)0), which is a no-op
    /// that gets optimized away by the compiler. This ensures zero performance
    /// impact in production builds while maintaining the same syntax.
    /// 
    /// The (void)0 cast prevents "unused variable" warnings if the condition
    /// contains variables that are only used in assertions.
    /// </summary>
    /// <param name="condition">Ignored in release builds</param>
#define ASSERT(condition) ((void)0)

/// <summary>
/// Release build version of ASSERT_MSG - completely removed for performance.
/// 
/// Like ASSERT, this expands to a no-op in release builds, ensuring that
/// both the condition evaluation and message formatting are eliminated
/// for optimal performance.
/// </summary>
/// <param name="condition">Ignored in release builds</param>
/// <param name="msg">Ignored in release builds</param>
#define ASSERT_MSG(condition, msg) ((void)0)
#endif

/// <summary>
/// Implementation Notes:
/// 
/// 1. Macro Design:
///    - Uses do-while(0) idiom for statement-like behavior
///    - Prevents issues with if-else statements without braces
///    - Ensures macro expansion is treated as single statement
/// 
/// 2. Condition Stringification:
///    - #condition converts the condition to a string literal
///    - Allows logging the exact code that failed
///    - Helpful for debugging without looking at source
/// 
/// 3. Build Configuration:
///    - _DEBUG is defined by Visual Studio in debug builds
///    - Assertions are completely removed in release builds
///    - No runtime overhead in production code
/// 
/// 4. Error Handling:
///    - Uses std::abort() for immediate termination
///    - Generates core dump on supported systems
///    - Stops execution at the exact point of failure
/// 
/// 5. Thread Safety:
///    - LOG_ERROR calls should be thread-safe (depends on logger implementation)
///    - std::abort() is thread-safe and terminates entire process
/// </summary>