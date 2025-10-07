#pragma once
#include "Vector2.h"
#include "Core/Serializable.h"

namespace neu {
    /// <summary>
    /// Represents a 2D transformation containing position, rotation, and uniform scale.
    /// 
    /// The Transform struct is a fundamental component in 2D graphics and game engines,
    /// encapsulating the three basic transformations that define an object's spatial
    /// properties in 2D space. It implements ISerializable to support loading transform
    /// data from configuration files and save systems.
    /// 
    /// Mathematical Foundation:
    /// - Position: Translation in 2D space (x, y coordinates)
    /// - Rotation: Angular orientation in radians around the object's center
    /// - Scale: Uniform scaling factor applied to both x and y dimensions
    /// 
    /// Transformation Order:
    /// The typical transformation order is Scale → Rotate → Translate (SRT),
    /// which means scaling happens first, then rotation, then translation.
    /// This order ensures intuitive behavior where objects scale around their
    /// center, rotate around their center, then move to their final position.
    /// 
    /// Common Use Cases:
    /// - Game object positioning and orientation (players, enemies, items)
    /// - UI element layout and positioning
    /// - Sprite rendering transformations
    /// - Physics body transformations
    /// - Camera transformations for view matrices
    /// - Particle system transformations
    /// - Animation keyframe data
    /// 
    /// Integration with Graphics Systems:
    /// - Can be converted to transformation matrices for rendering
    /// - Compatible with 2D graphics APIs (OpenGL, DirectX, etc.)
    /// - Supports hierarchical transformations (parent-child relationships)
    /// - Enables efficient batch transformations for multiple objects
    /// 
    /// Performance Considerations:
    /// - Lightweight structure with minimal memory overhead
    /// - Efficient for frequent updates (movement, rotation, scaling)
    /// - Matrix conversion can be cached when transformation doesn't change
    /// - Suitable for real-time applications and game loops
    /// </summary>
    struct Transform : public ISerializable {
        /// <summary>
        /// 2D position in world space coordinates.
        /// 
        /// Represents the translation component of the transformation, defining where
        /// the object is located in 2D space. The position is typically measured from
        /// the world origin (0, 0) and represents the object's center or pivot point.
        /// 
        /// Coordinate System:
        /// - X-axis: Horizontal position (positive = right, negative = left)
        /// - Y-axis: Vertical position (positive = up, negative = down in standard coordinates)
        /// 
        /// Default value: (0, 0) - positioned at the world origin
        /// 
        /// Common uses:
        /// - Object placement in game worlds
        /// - UI element positioning
        /// - Waypoint and path coordinates
        /// - Spawn locations and target positions
        /// </summary>
        vec2 position{ 0, 0 };

        /// <summary>
        /// Rotation angle in radians around the object's center.
        /// 
        /// Defines the angular orientation of the object in 2D space. The rotation
        /// is measured counter-clockwise from the positive X-axis, following standard
        /// mathematical convention.
        /// 
        /// Angular Measurements:
        /// - 0 radians: Pointing right (positive X direction)
        /// - π/2 radians (90°): Pointing up (positive Y direction)
        /// - π radians (180°): Pointing left (negative X direction)  
        /// - 3π/2 radians (270°): Pointing down (negative Y direction)
        /// 
        /// Range: Typically kept in [0, 2π] or [-π, π] range, though any value is valid
        /// Default value: 0 (no rotation, pointing right)
        /// 
        /// Common uses:
        /// - Sprite orientation (character facing direction)
        /// - Projectile trajectory angles
        /// - UI element rotation effects
        /// - Camera rotation for tilted views
        /// - Animation rotation keyframes
        /// 
        /// Note: If your coordinate system uses Y-down (common in computer graphics),
        /// positive rotation may appear clockwise on screen.
        /// </summary>
        float rotation = 0;

        /// <summary>
        /// Uniform scale factor applied to both X and Y dimensions.
        /// 
        /// Controls the size of the object relative to its original dimensions.
        /// This is a uniform scale, meaning the same factor is applied to both
        /// width and height, preserving the object's aspect ratio.
        /// 
        /// Scale Values:
        /// - 1.0: Original size (no scaling)
        /// - > 1.0: Larger than original (e.g., 2.0 = double size)
        /// - 0.0 < scale < 1.0: Smaller than original (e.g., 0.5 = half size)
        /// - 0.0: Invisible/degenerate (not recommended)
        /// - < 0.0: Flipped and scaled (negative values flip the object)
        /// 
        /// Default value: 1 (original size, no scaling)
        /// 
        /// Common uses:
        /// - Object size variations (power-ups, different enemy sizes)
        /// - Animation effects (growing, shrinking, pulsing)
        /// - UI scaling for different screen resolutions
        /// - Zoom effects and magnification
        /// - Size-based game mechanics (small vs large objects)
        /// 
        /// Performance note: Non-uniform scaling (different X/Y factors) would
        /// require a more complex transformation representation but is not
        /// supported by this simple Transform structure.
        /// </summary>
        float scale = 1;

        /// <summary>
        /// Default constructor creating an identity transform.
        /// 
        /// Creates a Transform with default values:
        /// - Position: (0, 0) - at world origin
        /// - Rotation: 0 radians - no rotation
        /// - Scale: 1 - original size
        /// 
        /// This represents the identity transformation, where objects appear
        /// at the origin with no modifications to their original state.
        /// </summary>
        Transform() = default;

        /// <summary>
        /// Parameterized constructor for creating transforms with specified values.
        /// 
        /// Allows creation of Transform objects with custom position, rotation, and scale
        /// in a single constructor call. Rotation and scale parameters are optional,
        /// defaulting to no rotation and original scale if not specified.
        /// 
        /// This constructor is particularly useful for:
        /// - Setting up initial object positions
        /// - Creating transforms from known values
        /// - Factory methods that create positioned objects
        /// - Animation keyframe initialization
        /// 
        /// Example usage:
        /// ```cpp
        /// Transform playerTransform({100.0f, 200.0f}, math::pi / 4, 1.5f);  // Position, 45° rotation, 1.5x scale
        /// Transform simpleTransform({50.0f, 75.0f});  // Position only, default rotation and scale
        /// ```
        /// </summary>
        /// <param name="position">The 2D position in world coordinates</param>
        /// <param name="rotation">The rotation angle in radians (default: 0 = no rotation)</param>
        /// <param name="scale">The uniform scale factor (default: 1 = original size)</param>
        Transform(const vec2& position, float rotation = 0, float scale = 1) :
            position{ position },
            rotation{ rotation },
            scale{ scale }
        {
        }

        /// <summary>
        /// Deserializes transform data from serialized format.
        /// 
        /// Implements the ISerializable interface to load Transform data from
        /// configuration files, save games, or other serialized sources.
        /// This enables data-driven object placement and transform configuration.
        /// 
        /// Expected serialized format:
        /// ```json
        /// {
        ///     "position": {x: 100.0, y: 200.0},
        ///     "rotation": 1.5708,  // π/2 radians (90 degrees)
        ///     "scale": 1.5
        /// }
        /// ```
        /// 
        /// The method uses SERIAL_READ macros to safely extract data with
        /// default values for missing properties:
        /// - Missing position defaults to (0, 0)
        /// - Missing rotation defaults to 0 radians
        /// - Missing scale defaults to 1.0
        /// 
        /// This robust loading ensures transforms work even with incomplete
        /// or legacy data files, providing graceful degradation.
        /// 
        /// Common uses:
        /// - Loading object positions from level definition files
        /// - Restoring transform state from save games
        /// - Reading transform data from configuration files
        /// - Deserializing network transform updates
        /// </summary>
        /// <param name="value">Serialized data containing transform information</param>
        void Read(const serial_data_t& value) override;
    };
}