#include "PostProcessComponent.h"
#include "Engine.h"

namespace neu {
	FACTORY_REGISTER(PostProcessComponent)

		void PostProcessComponent::Update(float dt) {
	}

	void PostProcessComponent::Apply(Program& program) {
		program.SetUniform("u_parameters", (uint32_t)parameters);
		program.SetUniform("u_colorTint", colorTint);
		program.SetUniform("u_time", neu::GetEngine().GetTime().GetTime()); //< set u_time with the engine time GetTime()
		program.SetUniform("u_blend", blend);
	}

	void PostProcessComponent::Read(const serial_data_t& value) {
		Object::Read(value);
	}

	void PostProcessComponent::UpdateGui() {
		uint32_t iparameters = (uint32_t)parameters;

		bool parameter = iparameters & (uint32_t)Parameters::GrayScale;
		if (ImGui::Checkbox("Gray Scale", &parameter)) {
			if (parameter) iparameters |= (uint32_t)Parameters::GrayScale;
			else iparameters &= ~(uint32_t)Parameters::GrayScale;
		}

		parameter = iparameters & (uint32_t)Parameters::ColorTint;
		if (ImGui::Checkbox("Color Tint", &parameter)) {
			if (parameter) iparameters |= (uint32_t)Parameters::ColorTint;
			else iparameters &= ~(uint32_t)Parameters::ColorTint;
		}

		parameter = iparameters & (uint32_t)Parameters::ScanLine;
		if (ImGui::Checkbox("Scan Line", &parameter)) {
			if (parameter) iparameters |= (uint32_t)Parameters::ScanLine;
			else iparameters &= ~(uint32_t)Parameters::ScanLine;
		}

		parameter = iparameters & (uint32_t)Parameters::Grain;
		if (ImGui::Checkbox("Grain", &parameter)) {
			if (parameter) iparameters |= (uint32_t)Parameters::Grain;
			else iparameters &= ~(uint32_t)Parameters::Grain;
		}

		parameter = iparameters & (uint32_t)Parameters::Invert;
		if (ImGui::Checkbox("Invert", &parameter)) {
			if (parameter) iparameters |= (uint32_t)Parameters::Invert;
			else iparameters &= ~(uint32_t)Parameters::Invert;
		}

			parameters = (Parameters)iparameters;

		ImGui::ColorEdit3("Color Tint", glm::value_ptr(colorTint));
		ImGui::SliderFloat("u_blend", &blend, 0.0f, 1.0f); //<set blend, range is 0 - 1>
	}
}