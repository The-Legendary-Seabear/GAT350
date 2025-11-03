#pragma once

namespace neu {
	class CameraComponent : public Component, GUI {
	public:
		CLASS_PROTOTYPE(CameraComponent)

		void Update(float dt) override;

		void SetPerspective(float fov, float aspect, float near, float far);
		void SetLookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up = { 0, 1, 0 });

		void SetProgram(Program& program);

		void Read(const serial_data_t& value) override;
		void UpdateGui() override;

	public:
		glm::mat4 projection{ 1 };
		glm::mat4 view{ 1 };

		float fov = 70.0f;
		float aspect = 0;
		float near = 0.1f;
		float far = 100.0f;
	};
}