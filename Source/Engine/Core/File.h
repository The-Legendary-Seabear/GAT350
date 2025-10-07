#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>

/// <summary>
/// File system utilities namespace providing cross-platform file and directory operations.
/// 
/// This namespace wraps std::filesystem functionality with simplified error handling
/// and convenient interfaces for common file operations. All functions use error_code
/// versions of filesystem operations to avoid exceptions and provide graceful error handling.
/// 
/// Key features:
/// - Cross-platform path handling
/// - Text and binary file I/O operations
/// - Directory traversal and listing
/// - Path manipulation utilities
/// - Safe error handling without exceptions
/// 
/// All path operations work with forward slashes on all platforms and automatically
/// handle platform-specific path separators internally.
/// </summary>
namespace neu::file {
    /// <summary>
    /// Retrieves the current working directory as a string.
    /// This is the directory from which relative paths are resolved and where
    /// the application was launched or last set via SetCurrentDirectory().
    /// </summary>
    /// <returns>A string containing the current working directory path, or an empty string if an error occurs</returns>
    std::string GetCurrentDirectory();

    /// <summary>
    /// Sets the current working directory to the specified path.
    /// This changes the base directory for relative path resolution. The path
    /// can be either relative (to current directory) or absolute.
    /// </summary>
    /// <param name="path">The path to set as the current working directory (relative or absolute)</param>
    /// <returns>True if the current directory was successfully changed, false if the path doesn't exist or access is denied</returns>
    bool SetCurrentDirectory(const std::string& path);

    /// <summary>
    /// Retrieves the file extension from a given file path.
    /// The extension includes the leading dot (e.g., ".txt", ".png", ".cpp").
    /// If the file has multiple extensions, only the last one is returned.
    /// </summary>
    /// <param name="path">The file path from which to extract the extension</param>
    /// <returns>The file extension including the leading dot, or an empty string if there is no extension</returns>
    std::string GetExtension(const std::string& path);

    /// <summary>
    /// Extracts the filename (including extension) from a given file path.
    /// This returns only the final component of the path, without any directory information.
    /// For example, "/path/to/file.txt" returns "file.txt".
    /// </summary>
    /// <param name="path">The file path from which to extract the filename</param>
    /// <returns>The filename component of the specified path as a string</returns>
    std::string GetFilename(const std::string& path);

    /// <summary>
    /// Checks whether a file or directory exists at the specified path.
    /// This function works for both files and directories and follows symbolic links.
    /// It uses error-safe filesystem operations to avoid exceptions.
    /// </summary>
    /// <param name="path">The path to the file or directory to check for existence</param>
    /// <returns>True if the file or directory exists and is accessible, false otherwise</returns>
    bool Exists(const std::string& path);

    /// <summary>
    /// Retrieves a list of regular files in the specified directory.
    /// This function performs a non-recursive directory scan, returning only files
    /// (not subdirectories, symbolic links, or special files) in the immediate directory.
    /// The returned paths are absolute paths to each file.
    /// </summary>
    /// <param name="path">The path to the directory to search for files</param>
    /// <returns>A vector containing the absolute paths of regular files found in the directory. Returns an empty vector if the directory doesn't exist or an error occurs</returns>
    std::vector<std::string> GetFilesInDirectory(const std::string& path);

    /// <summary>
    /// Retrieves a list of subdirectories within the specified path.
    /// This function performs a non-recursive directory scan, returning only directories
    /// (not files or other filesystem entries) in the immediate directory.
    /// The returned paths are absolute paths to each subdirectory.
    /// </summary>
    /// <param name="path">The path to the directory whose subdirectories are to be listed</param>
    /// <returns>A vector of strings containing the absolute paths of all subdirectories found. Returns an empty vector if the directory doesn't exist or an error occurs</returns>
    std::vector<std::string> GetDirectoriesIn(const std::string& path);

    /// <summary>
    /// Reads the contents of a text file into a string.
    /// This function reads the entire file into memory as a single string operation.
    /// It's suitable for reasonably-sized text files and handles different line endings automatically.
    /// The file is opened in text mode, which may perform line-ending conversion on some platforms.
    /// </summary>
    /// <param name="path">The path to the text file to be read</param>
    /// <param name="content">A reference to a string where the file's contents will be stored</param>
    /// <returns>True if the file was successfully opened and read, false if the file doesn't exist or cannot be accessed</returns>
    bool ReadTextFile(const std::string& path, std::string& content);

    /// <summary>
    /// Writes the specified content to a text file at the given path.
    /// Creates a new file if it doesn't exist, or overwrites/appends to an existing file.
    /// The file is opened in text mode, which may perform line-ending conversion on some platforms.
    /// Parent directories must already exist - this function does not create directory paths.
    /// </summary>
    /// <param name="path">The path to the file where the content will be written</param>
    /// <param name="content">The text content to write to the file</param>
    /// <param name="append">If true, appends to existing file; if false (default), overwrites the file</param>
    /// <returns>True if the file was successfully written, false if the path is invalid or write access is denied</returns>
    bool WriteTextFile(const std::string& path, const std::string& content, bool append = false);

    /// <summary>
    /// Reads the contents of a binary file into a vector of bytes.
    /// This function reads the entire file into memory in binary mode, preserving
    /// all bytes exactly as stored. It's suitable for any file type including images,
    /// executables, compressed files, or any non-text data.
    /// The vector is automatically resized to match the file size.
    /// </summary>
    /// <param name="path">The path to the binary file to read</param>
    /// <param name="data">A reference to a vector that will be resized and filled with the file's contents</param>
    /// <returns>True if the file was successfully opened and read, false if the file doesn't exist, cannot be accessed, or read errors occur</returns>
    bool ReadBinaryFile(const std::string& path, std::vector<uint8_t>& data);

    /// <summary>
    /// Writes binary data to a file at the specified path.
    /// Creates a new file or completely overwrites an existing file with the provided binary data.
    /// The data is written in binary mode, preserving all bytes exactly as provided.
    /// This function is suitable for writing any type of binary data including images, assets, or serialized objects.
    /// </summary>
    /// <param name="path">The path to the file where the data will be written</param>
    /// <param name="data">The binary data to write to the file</param>
    /// <returns>True if the file was successfully created and written, false if the path is invalid or write access is denied</returns>
    bool WriteBinaryFile(const std::string& path, const std::vector<uint8_t>& data);
}