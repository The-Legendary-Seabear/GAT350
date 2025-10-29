#pragma once
#include "Renderer/GUI.h"
#include "Core/Serializable.h"
#include <glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/euler_angles.hpp>

namespace neu {
    
    struct Transform : public ISerializable, GUI {
        
        glm::vec3 position{ 0, 0, 0 };
		glm::vec3 rotation{ 0, 0, 0 };
		glm::vec3 scale{ 1, 1, 1 };
        

        
        Transform() = default;
        Transform(const glm::vec3& position, const glm::vec3& rotation = {0, 0, 0}, const glm::vec3& scale = {1, 1, 1}) :
            position{ position },
            rotation{ rotation },
            scale{ scale }
        {
        }

        glm::mat4 GetMatrix() const {
            glm::mat4 mx = glm::mat4(1.0f);
            mx = glm::translate(mx, position);
            mx *= glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));//glm::rotate(mx, glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
            mx = glm::scale(mx, scale);

            return mx;
        }

        operator glm::mat4 () const {
			return GetMatrix();
        }
        
        void Read(const serial_data_t& value) override;

        void UpdateGui() override;
    };
}