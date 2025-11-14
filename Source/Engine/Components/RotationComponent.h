#pragma once

namespace neu {
	class RotationComponent : public Component {
	public:
		CLASS_PROTOTYPE(RotationComponent)

		void Update(float dt);

		void Read(const serial_data_t& value) override;
		void UpdateGui() override;

	public:
		glm::vec3 rotate{ 0 };
	};
}
