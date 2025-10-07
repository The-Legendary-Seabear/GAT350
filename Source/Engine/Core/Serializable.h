#pragma once
#include "Json.h"

namespace neu {
    using serial_data_t = serial::value_t;

    /// <summary>
    /// Abstract interface for objects that can be deserialized from JSON data.
    /// 
    /// This interface establishes a contract for objects that need to populate themselves
    /// from JSON configuration files, network data, or other serialized sources.
    /// It follows the common pattern of providing a standardized way to read object
    /// state from external data formats.
    /// 
    /// Design Philosophy:
    /// - Provides a uniform interface for JSON deserialization across different object types
    /// - Enables polymorphic deserialization through base class pointers
    /// - Separates serialization concerns from core object functionality
    /// - Facilitates configuration-driven object initialization
    /// 
    /// Common Use Cases:
    /// - Loading game entities from level definition files
    /// - Parsing configuration settings from JSON files
    /// - Deserializing network messages and API responses
    /// - Creating objects from save game data
    /// - Reading asset metadata and resource definitions
    /// 
    /// Example Implementation:
    /// ```cpp
    /// class Player : public ISerializable {
    /// public:
    ///     void Read(const serial_data_t& value) override {
    ///         m_name = json::Get(value, "name", std::string("DefaultPlayer"));
    ///         m_level = json::Get(value, "level", 1);
    ///         m_position = json::Get(value, "position", vec2(0, 0));
    ///     }
    /// private:
    ///     std::string m_name;
    ///     int m_level;
    ///     vec2 m_position;
    /// };
    /// ```
    /// </summary>
    class ISerializable {
    public:
        /// <summary>
        /// Virtual destructor to ensure proper cleanup of derived classes.
        /// This is essential for abstract base classes to guarantee that derived
        /// class destructors are called when objects are deleted through base pointers.
        /// 
        /// The default implementation allows derived classes to provide their own
        /// destructors while ensuring virtual dispatch works correctly during cleanup.
        /// </summary>
        virtual ~ISerializable() = default;

        /// <summary>
        /// Pure virtual method for reading object state from JSON data.
        /// 
        /// This method must be implemented by all concrete classes that inherit
        /// from ISerializable. The implementation should extract relevant data
        /// from the provided JSON value and use it to initialize or update
        /// the object's internal state.
        /// 
        /// Implementation Guidelines:
        /// - Handle missing JSON keys gracefully with default values
        /// - Validate input data and handle malformed JSON appropriately
        /// - Use the json utility functions (json::Get, json::Has, etc.) for safe data extraction
        /// - Consider backwards compatibility when adding new fields
        /// - Log warnings for unexpected or deprecated JSON structure
        /// - Maintain object invariants even with incomplete data
        /// 
        /// Error Handling Strategies:
        /// - Use default values for missing optional fields
        /// - Log errors for missing required fields
        /// - Consider throwing exceptions for critical parsing failures
        /// - Provide meaningful error messages with context information
        /// 
        /// Performance Considerations:
        /// - Cache frequently accessed JSON paths if performing multiple reads
        /// - Avoid unnecessary string comparisons and conversions
        /// - Consider lazy loading for expensive operations
        /// 
        /// Thread Safety:
        /// - This method should be called from a single thread during object initialization
        /// - Ensure any shared resources are properly synchronized
        /// </summary>
        /// <param name="value">
        /// The JSON value containing the serialized object data.
        /// This is typically a JSON object (dictionary/map) but could be any valid JSON type
        /// depending on how the object chooses to serialize itself. The implementation
        /// should validate the structure and extract the necessary data safely.
        /// </param>
        virtual void Read(const serial_data_t& value) = 0;
    };    
}