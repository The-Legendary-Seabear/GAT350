#pragma once
#include <glad/glad.h>
#include "Resources/Resource.h"


namespace neu {

class Shader : public Resource {
public:
	~Shader();

	bool Load(const std::string& filename, GLuint shaderType);

public:
	GLuint m_shader = 0;
};
}