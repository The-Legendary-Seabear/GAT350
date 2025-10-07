#include "Object.h"

namespace neu {
    /// <summary>
    /// Deserializes the base Object properties from serialized data.
    /// 
    /// This implementation handles the common properties that all Object-derived
    /// classes share. It uses serialization utility macros for safe data extraction with
    /// automatic default value handling.
    /// 
    /// Properties loaded:
    /// - name: Object identifier string (defaults to empty string if missing)
    /// - active: Object active state boolean (defaults to true if missing)
    /// 
    /// The SERIAL_READ macro provides:
    /// - Safe data extraction without throwing exceptions
    /// - Automatic type conversion where possible
    /// - Default value assignment for missing keys
    /// - Logging of any data extraction issues
    /// 
    /// This method is designed to be called by derived classes as the first step
    /// in their own Read() implementations, ensuring consistent loading of base
    /// object properties across the entire object hierarchy.
    /// 
    /// Example usage in derived classes:
    /// ```cpp
    /// void Player::Read(const serial_data_t& value) override {
    ///     Object::Read(value);  // Load base Object properties first
    ///     SERIAL_READ(value, health);
    ///     SERIAL_READ(value, experience);
    ///     // ... load other Player-specific properties
    /// }
    /// ```
    /// </summary>
    /// <param name="value">Serialized data containing the object's state information</param>
    void Object::Read(const serial_data_t& value) {
        // Load object name with empty string as default
        // This provides a unique identifier for debugging and object management
        SERIAL_READ(value, name);

        // Load active state with true as default
        // Controls whether the object participates in game systems
        SERIAL_READ(value, active);
    }
}