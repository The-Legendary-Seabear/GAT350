#pragma once
#include <glad/glad.h>
#include "Resources/Resource.h"


namespace neu {

class Shader : public Resource {
public:
	~Shader();

	bool Load(const std::string& filename, GLuint shaderType);
	void UpdateGui() override {}

public:
	GLuint m_shader = 0;
};
}