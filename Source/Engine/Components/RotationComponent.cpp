#include "RotationComponent.h"

namespace neu {
	FACTORY_REGISTER(RotationComponent)

	void RotationComponent::Update(float dt) {
		glm::quat q = owner->transform.rotationq * glm::quat{ glm::radians(rotate * dt) };
		owner->transform.SetRotationQuat(q);
	}

	void RotationComponent::Read(const serial_data_t& value) {
		SERIAL_READ(value, rotate);
	}

	void RotationComponent::UpdateGui() {
		ImGui::DragFloat3("Rotate", glm::value_ptr(rotate), 0.1f);
	}
}