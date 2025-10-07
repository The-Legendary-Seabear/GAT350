#include "Transform.h"

namespace neu {
    /// <summary>
    /// Deserializes Transform data from serialized format.
    /// 
    /// This method implements the ISerializable interface to load Transform properties
    /// from configuration data. It uses the SERIAL_READ macro system to safely extract
    /// transform values with automatic default value handling.
    /// 
    /// Loading Process:
    /// 1. Reads position as a vec2 (defaults to {0, 0} if missing)
    /// 2. Reads rotation as a float (defaults to 0 if missing)
    /// 3. Reads scale as a float (defaults to 1 if missing)
    /// 
    /// The SERIAL_READ macros provide:
    /// - Safe data extraction without exceptions
    /// - Automatic type conversion where appropriate
    /// - Default value assignment for missing keys
    /// - Logging of any data extraction issues
    /// 
    /// Expected serialized data format:
    /// ```json
    /// {
    ///     "position": ["x": 100.0, "y": 200.0],
    ///     "rotation": 1.5708,  // π/2 radians (90 degrees)
    ///     "scale": 1.5
    /// }
    /// ```
    /// 
    /// Graceful degradation:
    /// - If position is missing: object appears at world origin (0, 0)
    /// - If rotation is missing: object has no rotation (0 radians)
    /// - If scale is missing: object maintains original size (1.0 scale)
    /// 
    /// This robust loading ensures transforms work correctly even with:
    /// - Incomplete data files
    /// - Legacy format files missing newer properties
    /// - Malformed or corrupted data entries
    /// - Different serialization formats or versions
    /// 
    /// Use cases:
    /// - Loading object positions from level definition files
    /// - Restoring transform state from save game data
    /// - Reading initial transform values from configuration files
    /// - Deserializing network transform synchronization data
    /// </summary>
    /// <param name="value">Serialized data containing transform properties</param>
    void Transform::Read(const serial_data_t& value) {
        // Load position vector with default (0, 0) if missing
        // The vec2 handles x/y position components
        SERIAL_READ(value, position);

        // Load rotation angle with default 0 radians if missing
        // Rotation is stored and expected in radians for mathematical consistency
        SERIAL_READ(value, rotation);

        // Load scale factor with default 1.0 (original size) if missing
        // Scale of 1.0 maintains the object's original dimensions
        SERIAL_READ(value, scale);
    }
}