#pragma once

namespace neu {
	class PostProcessComponent : public Component {
	public:
		enum class Parameters : uint32_t {
			None = 0,
			GrayScale = (1 << 0),
			ColorTint = (1 << 1),
			ScanLine  = (1 << 2),
			Grain     = (1 << 3),
			Invert    = (1 << 4),
		};

	public:
		CLASS_PROTOTYPE(PostProcessComponent)

			void Update(float dt) override;
		void Apply(Program& program);

		void Read(const serial_data_t& value);
		void UpdateGui() override;

	public:
		Parameters parameters = Parameters::None;
		glm::vec3 colorTint{ 1, 1, 1 };
		float blend{ 1.0f };
	};
}