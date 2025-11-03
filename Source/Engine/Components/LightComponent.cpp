#include "LightComponent.h"

namespace neu {
	FACTORY_REGISTER(LightComponent)
	void LightComponent::Update(float dt) {

	}

	void LightComponent::SetProgram(Program& program, const std::string& name, const glm::mat4& view) {
		glm::vec3 position = glm::vec3(view * glm::vec4(owner->transform.position, 1));

		program.SetUniform(name + ".position", position);
		program.SetUniform(name + ".color", color);
	}

	void LightComponent::Read(const serial_data_t& value) {
		SERIAL_READ(value, color);

	}

	void LightComponent::UpdateGui() {
		ImGui::ColorEdit3("Color", glm::value_ptr(color));
	}
}