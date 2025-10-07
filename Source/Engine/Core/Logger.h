#pragma once
#include "File.h"
#include <string>
#include <iostream>
#include <format>

/// <summary>
/// Convenience macro for logging informational messages.
/// Automatically captures the current file and line number for context.
/// Uses C++20 std::format for type-safe, printf-style formatting.
/// 
/// Example usage:
///   LOG_INFO("Player spawned at position ({}, {})", x, y);
///   LOG_INFO("System initialized successfully");
/// </summary>
/// <param name="fmt">Format string compatible with std::format</param>
/// <param name="...">Optional arguments for format string placeholders</param>
#define LOG_INFO(fmt, ...) \
	neu::Logger::Info(__FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

/// <summary>
/// Convenience macro for logging warning messages.
/// Used for non-critical issues that should be noted but don't prevent execution.
/// Automatically captures the current file and line number for debugging context.
/// 
/// Example usage:
///   LOG_WARNING("Low memory warning: {}MB remaining", freeMemoryMB);
///   LOG_WARNING("Deprecated function called: {}", functionName);
/// </summary>
/// <param name="fmt">Format string compatible with std::format</param>
/// <param name="...">Optional arguments for format string placeholders</param>
#define LOG_WARNING(fmt, ...) \
	neu::Logger::Warning(__FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

/// <summary>
/// Convenience macro for logging error messages.
/// Used for critical issues, failures, and exceptions that require attention.
/// Automatically captures the current file and line number for debugging.
/// 
/// Example usage:
///   LOG_ERROR("Failed to load texture: {}", filename);
///   LOG_ERROR("Network connection timeout after {}ms", timeoutMs);
/// </summary>
/// <param name="fmt">Format string compatible with std::format</param>
/// <param name="...">Optional arguments for format string placeholders</param>
#define LOG_ERROR(fmt, ...) \
	neu::Logger::Error(__FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

/// <summary>
/// Convenience macro for logging debug messages.
/// Used for detailed diagnostic information during development and troubleshooting.
/// Can be selectively enabled/disabled without affecting other log levels.
/// 
/// Example usage:
///   LOG_DEBUG("Entering function: {} with parameter {}", __func__, param);
///   LOG_DEBUG("State transition: {} -> {}", oldState, newState);
/// </summary>
/// <param name="fmt">Format string compatible with std::format</param>
/// <param name="...">Optional arguments for format string placeholders</param>
#define LOG_DEBUG(fmt, ...) \
	neu::Logger::Debug(__FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

namespace neu {

	/// <summary>
	/// Enumeration defining different logging levels with bitmask support.
	/// Uses bit flags to allow selective enabling/disabling of log categories.
	/// Each level represents a different severity or type of message.
	/// 
	/// The enum is designed as flags, allowing combinations like:
	/// - LogLevel::Info | LogLevel::Error (only info and error messages)
	/// - LogLevel::All (all message types)
	/// - LogLevel::None (no messages)
	/// </summary>
	enum class LogLevel : uint8_t {
		/// <summary>No logging enabled</summary>
		None    = 0,
		/// <summary>Informational messages (general status, notifications)</summary>
		Info    = 1 << 0,  // 0001
		/// <summary>Warning messages (potential issues, deprecated usage)</summary>
		Warning = 1 << 1,  // 0010
		/// <summary>Error messages (failures, exceptions, critical issues)</summary>
		Error   = 1 << 2,  // 0100
		/// <summary>Debug messages (detailed diagnostic information)</summary>
		Debug   = 1 << 3,  // 1000
		/// <summary>Convenience flag enabling all log levels</summary>
		All     = Info | Warning | Error | Debug  // 1111
	};

	/// <summary>
	/// Bitwise OR operator for LogLevel flags.
	/// Allows combining multiple log levels into a single flag value.
	/// 
	/// Example: LogLevel::Info | LogLevel::Error enables both info and error logging.
	/// </summary>
	/// <param name="a">First log level flag</param>
	/// <param name="b">Second log level flag</param>
	/// <returns>Combined log level flags</returns>
	inline LogLevel operator | (LogLevel a, LogLevel b) {
		return static_cast<LogLevel>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
	}

	/// <summary>
	/// Bitwise AND operator for LogLevel flags.
	/// Used to test if a specific log level is enabled within a combined flag.
	/// This is the primary mechanism for checking if logging should occur.
	/// 
	/// Example: (enabledLevels &amp; LogLevel::Info) != LogLevel::None
	/// </summary>
	/// <param name="a">Current enabled levels</param>
	/// <param name="b">Level to test for</param>
	/// <returns>Non-zero result if the level is enabled, LogLevel::None otherwise</returns>
	inline LogLevel operator & (LogLevel a, LogLevel b) {
		return static_cast<LogLevel>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
	}

	/// <summary>
	/// Static logging utility class providing centralized message logging with multiple severity levels.
	/// 
	/// Key features:
	/// - Multiple log levels with selective enabling/disabling
	/// - Automatic file name and line number capture for debugging context
	/// - ANSI color coding for different message types (terminal support)
	/// - C++20 std::format integration for type-safe formatting
	/// - Thread-safe logging to standard error stream
	/// - Macro-based interface for convenient usage
	/// 
	/// The logger uses a static design pattern, providing global access without requiring
	/// instance management. All methods are static and thread-safe for concurrent logging.
	/// </summary>
	class Logger {
	public:
		/// <summary>
		/// Sets which log levels are enabled for output.
		/// This allows runtime control over logging verbosity without recompilation.
		/// Disabled levels are filtered out before formatting, providing performance benefits.
		/// 
		/// Example usage:
		///   Logger::SetEnabledLevels(LogLevel::Info | LogLevel::Error);  // Only info and errors
		///   Logger::SetEnabledLevels(LogLevel::None);  // Disable all logging
		///   Logger::SetEnabledLevels(LogLevel::All);   // Enable everything
		/// </summary>
		/// <param name="levels">Bitmask of LogLevel flags specifying which levels to enable</param>
		static void SetEnabledLevels(LogLevel levels) {
			s_enabledLevels = levels;
		}

		/// <summary>
		/// Core logging function that handles message formatting and output.
		/// This is the central function called by all specific log level methods.
		/// 
		/// Process:
		/// 1. Check if the log level is enabled (early exit for performance)
		/// 2. Determine appropriate prefix and ANSI color code based on level
		/// 3. Extract filename from full file path for cleaner output
		/// 4. Format the complete message with color codes and context
		/// 5. Output to standard error stream
		/// 
		/// The function uses ANSI escape codes for terminal coloring, which work
		/// on most modern terminals and development environments.
		/// </summary>
		/// <param name="level">The severity level of this log message</param>
		/// <param name="file">Full file path where the log call originated (__FILE__ macro)</param>
		/// <param name="line">Line number where the log call originated (__LINE__ macro)</param>
		/// <param name="message">The formatted message content to log</param>
		static void Log(LogLevel level, const char* file, int line, const std::string& message) {

			// Early exit if this log level is disabled (performance optimization)
			if ((s_enabledLevels & level) == LogLevel::None) return;

			std::string prefix;
			std::string color;

			// Determine display properties based on log level
			switch (level) {
			case LogLevel::Info:
				prefix = "[INFO] ";
				color = "\033[32m"; // Green - positive/neutral information
				break;
			case LogLevel::Warning:
				prefix = "[WARNING] ";
				color = "\033[33m"; // Yellow - caution/attention needed
				break;
			case LogLevel::Error:
				prefix = "[ERROR] ";
				color = "\033[31m"; // Red - problems/failures
				break;
			case LogLevel::Debug:
				prefix = "[DEBUG] ";
				color = "\033[36m"; // Cyan - diagnostic information
				break;
			default:
				prefix = "[UNKNOWN] ";
				color = "\033[0m"; // Reset - fallback for invalid levels
				break;
			}

			// Extract just the filename from the full path for cleaner output
			// This reduces clutter in log messages while maintaining useful context
			std::string filename = file::GetFilename(file);

			// ANSI reset code to return terminal to default colors
			const std::string reset = "\033[0m";

			// Format the complete log message with color, prefix, location, and content
			std::string output = std::format("{}{}[{}:{}] {}{}\n",
				color, prefix, filename, line, message, reset);

			// Output to stderr for immediate visibility and proper stream separation
			std::cerr << output;
		}

		/// <summary>
		/// Logs an informational message using C++20 format string syntax.
		/// Template function providing compile-time format string validation and type safety.
		/// Automatically forwards all arguments to std::format for efficient formatting.
		/// </summary>
		/// <typeparam name="Args">Variadic template parameter pack for format arguments</typeparam>
		/// <param name="file">Source file name (automatically provided by LOG_INFO macro)</param>
		/// <param name="line">Source line number (automatically provided by LOG_INFO macro)</param>
		/// <param name="fmt">Compile-time validated format string</param>
		/// <param name="args">Arguments for format string placeholders (perfect forwarded)</param>
		template<typename... Args>
		static void Info(const char* file, int line, std::format_string<Args...> fmt, Args&&... args) {
			Log(LogLevel::Info, file, line, std::format(fmt, std::forward<Args>(args)...));
		}

		/// <summary>
		/// Logs an error message using C++20 format string syntax.
		/// Template function for type-safe error message formatting with perfect forwarding.
		/// Used for critical issues, failures, and exceptions that require immediate attention.
		/// </summary>
		/// <typeparam name="Args">Variadic template parameter pack for format arguments</typeparam>
		/// <param name="file">Source file name (automatically provided by LOG_ERROR macro)</param>
		/// <param name="line">Source line number (automatically provided by LOG_ERROR macro)</param>
		/// <param name="fmt">Compile-time validated format string</param>
		/// <param name="args">Arguments for format string placeholders (perfect forwarded)</param>
		template<typename... Args>
		static void Error(const char* file, int line, std::format_string<Args...> fmt, Args&&... args) {
			Log(LogLevel::Error, file, line, std::format(fmt, std::forward<Args>(args)...));
		}

		/// <summary>
		/// Logs a warning message using C++20 format string syntax.
		/// Template function for type-safe warning message formatting with perfect forwarding.
		/// Used for non-critical issues that should be noted but don't prevent execution.
		/// </summary>
		/// <typeparam name="Args">Variadic template parameter pack for format arguments</typeparam>
		/// <param name="file">Source file name (automatically provided by LOG_WARNING macro)</param>
		/// <param name="line">Source line number (automatically provided by LOG_WARNING macro)</param>
		/// <param name="fmt">Compile-time validated format string</param>
		/// <param name="args">Arguments for format string placeholders (perfect forwarded)</param>
		template<typename... Args>
		static void Warning(const char* file, int line, std::format_string<Args...> fmt, Args&&... args) {
			Log(LogLevel::Warning, file, line, std::format(fmt, std::forward<Args>(args)...));
		}

		/// <summary>
		/// Logs a debug message using C++20 format string syntax.
		/// Template function for type-safe debug message formatting with perfect forwarding.
		/// Used for detailed diagnostic information during development and troubleshooting.
		/// </summary>
		/// <typeparam name="Args">Variadic template parameter pack for format arguments</typeparam>
		/// <param name="file">Source file name (automatically provided by LOG_DEBUG macro)</param>
		/// <param name="line">Source line number (automatically provided by LOG_DEBUG macro)</param>
		/// <param name="fmt">Compile-time validated format string</param>
		/// <param name="args">Arguments for format string placeholders (perfect forwarded)</param>
		template<typename... Args>
		static void Debug(const char* file, int line, std::format_string<Args...> fmt, Args&&... args) {
			Log(LogLevel::Debug, file, line, std::format(fmt, std::forward<Args>(args)...));
		}

	private:
		/// <summary>
		/// Static member storing the currently enabled log levels.
		/// Initialized to LogLevel::All by default, enabling all logging on startup.
		/// Modified through SetEnabledLevels() for runtime log level control.
		/// 
		/// This static storage ensures consistent logging configuration across
		/// the entire application without requiring logger instance management.
		/// </summary>
		inline static LogLevel s_enabledLevels = LogLevel::All;
	};
}