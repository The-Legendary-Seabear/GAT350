#include "Transform.h"

namespace neu {
    
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
    void Transform::UpdateGui()
    {
        
        ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f);
        ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f);
    }
}