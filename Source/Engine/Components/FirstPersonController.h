#pragma once

namespace neu {
	class FirstPersonController : public Component {
	public:
		CLASS_PROTOTYPE(FirstPersonController)

			bool Start() override;
		void Update(float dt) override;

		void Read(const serial_data_t& value) override;
		void UpdateGui() override;

	public:
		float sensitivity = 0.1f; // mouse sensitity
		float speed = 3; // movement speed
		float yaw = 0;
		float pitch = 0;
	};
}