#pragma once
#include <string>
#include <algorithm>
#include <cctype>

namespace neu {
    /// <summary>
    /// Converts all characters in a string to lowercase.
    /// Creates a copy of the input string and transforms each character using std::tolower.
    /// This function is safe for ASCII characters and preserves the original string.
    /// </summary>
    /// <param name="str">The input string to convert (passed by const reference for efficiency)</param>
    /// <returns>A new string with all characters converted to lowercase</returns>
    inline std::string toLower(const std::string& str) {
        // Create a copy to avoid modifying the original string
        std::string result = str;

        // Convert each character to lowercase using std::tolower
        // Range-based for loop with reference to modify characters in-place
        for (char& c : result) {
            c = std::tolower(c);
        }

        return result;
    }

    /// <summary>
    /// Converts all characters in a string to uppercase.
    /// Creates a copy of the input string and transforms each character using std::toupper.
    /// This function is safe for ASCII characters and preserves the original string.
    /// </summary>
    /// <param name="str">The input string to convert (passed by const reference for efficiency)</param>
    /// <returns>A new string with all characters converted to uppercase</returns>
    inline std::string toUpper(const std::string& str) {
        // Create a copy to avoid modifying the original string
        std::string result = str;

        // Convert each character to uppercase using std::toupper
        // Range-based for loop with reference to modify characters in-place
        for (char& c : result) {
            c = std::toupper(c);
        }

        return result;
    }

    /// <summary>
    /// Performs a case-insensitive comparison between two strings.
    /// First checks if the strings have the same length (optimization),
    /// then compares each character pair using lowercase conversion.
    /// This is more efficient than converting entire strings to lowercase.
    /// </summary>
    /// <param name="str1">The first string to compare</param>
    /// <param name="str2">The second string to compare</param>
    /// <returns>True if the strings are equal when case is ignored, false otherwise</returns>
    inline bool equalsIgnoreCase(const std::string& str1, const std::string& str2) {
        // Early exit: if lengths differ, strings cannot be equal
        if (str1.length() != str2.length()) return false;

        // Use std::equal with custom comparison lambda
        // Compares each character pair after converting both to lowercase
        return std::equal(str1.begin(), str1.end(), str2.begin(), [](char a, char b) {
            return (std::tolower(a) == std::tolower(b));
            });
    }
}