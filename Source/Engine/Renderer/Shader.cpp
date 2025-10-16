#include "EnginePCH.h"
#include "Shader.h"
 
namespace neu {
	Shader::~Shader() {
		if (m_shader) glDeleteShader(m_shader);
	}
 
	bool Shader::Load(const std::string& filename, GLuint shaderType) {
		std::string source;
		neu::file::ReadTextFile(filename, source);
		const char* source_cstr = source.c_str();
 
		m_shader = glCreateShader(shaderType);
		glShaderSource(m_shader, 1, &source_cstr, NULL);
		glCompileShader(m_shader);
		GLint success;
		glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			std::string infoLog(512, '\0');  // pre-allocate space
			GLsizei length;
			glGetShaderInfoLog(m_shader, (GLsizei)infoLog.size(), &length, &infoLog[0]);
			infoLog.resize(length);
 
			LOG_WARNING("Shader compilation failed: {}", infoLog);
 
			// delete shader
			glDeleteShader(m_shader);
			m_shader = 0;
 
			return false;
		}
 
		return true;
	}
}