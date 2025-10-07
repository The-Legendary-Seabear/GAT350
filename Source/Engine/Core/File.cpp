#include "File.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>

namespace neu::file {
    /// <summary>
    /// Retrieves the current working directory as a string.
    /// Uses std::filesystem::current_path() with error handling to avoid exceptions.
    /// </summary>
    /// <returns>A string containing the current working directory, or an empty string if an error occurs.</returns>
    std::string GetCurrentDirectory() {
        std::error_code ec;
        // Use error_code version to avoid exceptions
        auto path = std::filesystem::current_path(ec);
        // Return empty string on error, otherwise convert path to string
        return ec ? std::string{} : path.string();
    }

    /// <summary>
    /// Sets the current working directory to the specified path.
    /// Changes the working directory for the current process, affecting relative path resolution.
    /// </summary>
    /// <param name="path">The path to set as the current working directory.</param>
    /// <returns>True if the current directory was successfully set; false otherwise.</returns>
    bool SetCurrentDirectory(const std::string& path) {
        std::error_code ec;
        // Attempt to change current directory using error_code version
        std::filesystem::current_path(path, ec);

        // Return true if no error occurred
        return !ec;
    }

    /// <summary>
    /// Retrieves the file extension from a given file path.
    /// Extracts the extension using filesystem path parsing, including the leading dot.
    /// </summary>
    /// <param name="path">The file path as a string.</param>
    /// <returns>The file extension, including the leading dot, or an empty string if there is no extension.</returns>
    std::string GetExtension(const std::string& path) {
        // Create filesystem path object for parsing
        std::filesystem::path p(path);
        // Extract and return extension (includes the dot)
        return p.extension().string();
    }

    /// <summary>
    /// Extracts the filename from a given file path.
    /// Returns the final component of the path, including the file extension.
    /// </summary>
    /// <param name="path">The file path from which to extract the filename.</param>
    /// <returns>The filename component of the specified path as a string.</returns>
    std::string GetFilename(const std::string& path) {
        // Create filesystem path object for parsing
        std::filesystem::path p(path);
        // Extract and return the filename component
        return p.filename().string();
    }

    /// <summary>
    /// Checks whether a file or directory exists at the specified path.
    /// Uses error-safe filesystem operations to check existence without throwing exceptions.
    /// </summary>
    /// <param name="path">The path to the file or directory to check.</param>
    /// <returns>True if the file or directory exists and no error occurred; otherwise, false.</returns>
    bool Exists(const std::string& path) {
        std::error_code ec;
        // Check existence using error_code version to avoid exceptions
        bool result = std::filesystem::exists(path, ec);

        // Return true only if no error occurred AND the path exists
        return !ec && result;
    }

    /// <summary>
    /// Retrieves a list of regular files in the specified directory.
    /// Performs non-recursive directory iteration, filtering for regular files only.
    /// </summary>
    /// <param name="path">The path to the directory to search for files.</param>
    /// <returns>A vector containing the paths of regular files found in the directory. Returns an empty vector if an error occurs.</returns>
    std::vector<std::string> GetFilesInDirectory(const std::string& path) {
        std::vector<std::string> files;
        std::error_code ec;

        // Create directory iterator with error handling
        auto iter = std::filesystem::directory_iterator(path, ec);
        if (ec) return files; // Return empty vector on error (directory doesn't exist, no permissions, etc.)

        // Iterate through directory entries
        for (const auto& entry : iter) {
            // Check if entry is a regular file (not directory, symlink, etc.)
            if (entry.is_regular_file(ec) && !ec) {
                // Add the full path of the file to results
                files.push_back(entry.path().string());
            }
            // Continue iteration even if individual entry check fails
        }

        return files;
    }

    /// <summary>
    /// Retrieves a list of directories within the specified path.
    /// Performs non-recursive directory iteration, filtering for directories only.
    /// </summary>
    /// <param name="path">The path to the directory whose subdirectories are to be listed.</param>
    /// <returns>A vector of strings containing the paths of all subdirectories found in the specified directory. Returns an empty vector if an error occurs.</returns>
    std::vector<std::string> GetDirectoriesIn(const std::string& path) {
        std::vector<std::string> directories;
        std::error_code ec;

        // Create directory iterator with error handling
        auto iter = std::filesystem::directory_iterator(path, ec);
        if (ec) return directories; // Return empty vector on error

        // Iterate through directory entries
        for (const auto& entry : iter) {
            // Check if entry is a directory
            if (entry.is_directory(ec) && !ec) {
                // Add the full path of the directory to results
                directories.push_back(entry.path().string());
            }
            // Continue iteration even if individual entry check fails
        }

        return directories;
    }

    /// <summary>
    /// Reads the contents of a text file into a string.
    /// Opens file in text mode and reads entire contents using stringstream for efficiency.
    /// </summary>
    /// <param name="path">The path to the text file to be read.</param>
    /// <param name="content">A reference to a string where the file's contents will be stored.</param>
    /// <returns>True if the file was successfully read; false otherwise.</returns>
    bool ReadTextFile(const std::string& path, std::string& content) {
        // Open file in text mode (default)
        std::ifstream file(path);
        if (!file.is_open()) {
            return false; // File doesn't exist or cannot be opened
        }

        // Read entire file into stringstream for efficient string building
        std::stringstream buffer;
        buffer << file.rdbuf(); // Stream the entire file buffer
        content = buffer.str(); // Convert to string

        // File automatically closed by destructor
        return true;
    }

    /// <summary>
    /// Writes the specified content to a text file at the given path.
    /// Opens file in text mode with configurable append/overwrite behavior.
    /// </summary>
    /// <param name="path">The path to the file where the content will be written.</param>
    /// <param name="content">The text content to write to the file.</param>
    /// <param name="append">If true, appends to existing file; if false, overwrites the file.</param>
    /// <returns>True if the file was successfully written; false otherwise.</returns>
    bool WriteTextFile(const std::string& path, const std::string& content, bool append) {
        // Set open mode based on append parameter
        std::ios::openmode mode = append ? std::ios::app : std::ios::out;
        std::ofstream file(path, mode);

        if (!file.is_open()) {
            return false; // Cannot create/open file for writing
        }

        // Write content to file
        file << content;

        // File automatically closed and flushed by destructor
        return true;
    }

    /// <summary>
    /// Reads the contents of a binary file into a vector of bytes.
    /// Opens file in binary mode and reads entire contents preserving all bytes exactly.
    /// </summary>
    /// <param name="path">The path to the binary file to read.</param>
    /// <param name="data">A reference to a vector that will be filled with the file's contents.</param>
    /// <returns>True if the file was successfully read; false otherwise.</returns>
    bool ReadBinaryFile(const std::string& path, std::vector<uint8_t>& data) {
        // Open file in binary mode to preserve all bytes
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false; // File doesn't exist or cannot be opened
        }

        // Get file size by seeking to end
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg); // Return to beginning

        // Check if seek operations succeeded
        if (!file.good()) {
            return false; // File stream is in error state
        }

        // Resize vector to accommodate entire file
        data.resize(size);

        // Read entire file into vector's data buffer
        // reinterpret_cast is safe here as we're reading raw bytes
        file.read(reinterpret_cast<char*>(data.data()), size);

        // Verify read was successful
        return file.good();
    }

    /// <summary>
    /// Writes binary data to a file at the specified path.
    /// Creates/overwrites file in binary mode, preserving all bytes exactly as provided.
    /// </summary>
    /// <param name="path">The path to the file where the data will be written.</param>
    /// <param name="data">The binary data to write to the file.</param>
    /// <returns>True if the file was successfully written; false otherwise.</returns>
    bool WriteBinaryFile(const std::string& path, const std::vector<uint8_t>& data) {
        // Open file in binary mode for exact byte preservation
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false; // Cannot create/open file for writing
        }

        // Write vector data directly to file
        // reinterpret_cast is safe here as we're writing raw bytes
        file.write(reinterpret_cast<const char*>(data.data()), data.size());

        // Return success status (file automatically closed by destructor)
        return file.good();
    }
}