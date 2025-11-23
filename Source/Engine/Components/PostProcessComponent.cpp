#include "PostProcessComponent.h"

namespace neu {
	FACTORY_REGISTER(PostProcessComponent)

	void PostProcessComponent::Update(float dt)	{
	}

	void PostProcessComponent::Apply(Program& program) {
		program.SetUniform("u_parameters", (uint32_t)parameters);
		program.SetUniform("u_colorTint", colorTint);
	}

	void PostProcessComponent::Read(const serial_data_t& value) {
		Object::Read(value);
	}

	void PostProcessComponent::UpdateGui() {
		ImGui::ColorEdit3("Color Tint", glm::value_ptr(colorTint));
	}
}