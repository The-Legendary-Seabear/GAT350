#pragma once
#include "GUI.h"
#include "Resources/Resource.h"
#include <../glm/glm.hpp>

namespace neu {
	class Program;

	class Texture;

	class Material : public Resource, GUI {
	public:
		Material() = default;
		~Material() = default;

		bool Load(const std::string& filename);
		void Bind();

	public:
		float shininess{ 2 };
		glm::vec2 tiling{ 1, 1 };
		glm::vec2 offset{ 0, 0 };

		
		res_t<Texture> baseMap;
		res_t<Texture> specularMap;
		res_t<Program> program;
		glm::vec3 baseColor{ 1, 1, 1 };


		
		void UpdateGui() override;

	};
}

