#include "CameraComponent.h"
#include "Engine.h"

namespace neu {
	FACTORY_REGISTER(CameraComponent)

	void CameraComponent::Update(float dt) {
		view = (shadowCamera)
			? glm::lookAt(owner->transform.position, owner->transform.position + owner->transform.Forward(), owner->transform.Up())
			: glm::lookAt(owner->transform.position, owner->transform.position + owner->transform.Forward(), owner->transform.Up());
		projection = (projectionType == ProjectionType::Perspective)
			? glm::perspective(glm::radians(fov), aspect, near, far)
			: glm::ortho(-size * aspect, size * aspect, -size, size, near, far);
	}

	void CameraComponent::Clear() {
		glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1);
		GLbitfield bits = 0;
			if (clearColorBuffer) bits |= GL_COLOR_BUFFER_BIT;
			if (clearDepthBuffer) bits |= GL_DEPTH_BUFFER_BIT;
		glClear(bits);
	}

	void CameraComponent::SetPerspective(float fov, float aspect, float near, float far) {
		this->fov = fov;
		this->aspect = aspect;
		this->near = near;
		this->far = far;

		//projection = glm::perspective(glm::radians(fov), aspect, near, far);
	}

	void CameraComponent::SetLookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
		view = glm::lookAt(eye, center, up);
	}

	void CameraComponent::SetProgram(Program& program) {
		program.SetUniform("u_view", view);
		program.SetUniform("u_projection", projection);
	}

	void CameraComponent::Read(const serial_data_t& value) {
		SERIAL_READ(value, fov);
		if (!SERIAL_READ(value, aspect)) aspect = GetEngine().GetRenderer().GetWidth() / (float)GetEngine().GetRenderer().GetHeight();
		SERIAL_READ(value, near);
		SERIAL_READ(value, far);
		SERIAL_READ(value, size);

		SERIAL_READ(value, backgroundColor);
		SERIAL_READ(value, clearColorBuffer);
		SERIAL_READ(value, clearDepthBuffer);

		SERIAL_READ(value, shadowCamera);
		std::string projectionTypeName;
		SERIAL_READ_NAME(value, "projectionType", projectionTypeName);
		if (projectionTypeName.empty() && equalsIgnoreCase(projectionTypeName, "orthographic")) {
			projectionType = ProjectionType::Orthographic;
		}
		

		std::string outputTextureName;
		SERIAL_READ_NAME(value, "outputTexture", outputTextureName);
		if (!outputTextureName.empty()) {
			outputTexture = Resources().Get<RenderTexture>(outputTextureName);
		}
	}

	void CameraComponent::UpdateGui() {
		const char* types[] = { "Perspective", "Orthographic" };
		ImGui::Combo("Projection", (int*)&projectionType, types, 2);

		if (projectionType == ProjectionType::Perspective) {
		ImGui::DragFloat("FOV", &fov, 0.1f, 10.0f, 100.0f);
			
		}
		else {
			ImGui::DragFloat("Size", &size, 0.1f, 1.0f);
		}

		ImGui::DragFloat("Aspect", &aspect, 0.1f);
		ImGui::DragFloat("Near", &near, 0.1f);
		ImGui::DragFloat("Far", &far, 0.1f);

		ImGui::ColorEdit3("Background", glm::value_ptr(backgroundColor));
		ImGui::Checkbox("Clear Color", &clearColorBuffer);
		ImGui::Checkbox("Clear Depth", &clearDepthBuffer);
	}
}