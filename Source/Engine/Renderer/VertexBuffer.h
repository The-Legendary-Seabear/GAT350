#pragma once
#include "Resources/Resource.h"
#include <glad/glad.h>
#include <vector>

namespace neu
{
	class VertexBuffer : public Resource
	{
	public:
		VertexBuffer();
		virtual ~VertexBuffer();

		void Draw(GLenum primitiveType = GL_TRIANGLES);
		void Bind() { glBindVertexArray(m_vao); }

		void CreateVertexBuffer(GLsizei size, GLsizei count, GLvoid* data);
		void CreateIndexBuffer(GLenum indexType, GLsizei count, GLvoid* data);

		void SetAttribute(GLuint index, GLint size, GLsizei stride, GLuint offset);

	protected:
		// vertex array
		GLuint m_vao = 0;   // vertex array object

		// vertex buffer
		GLuint m_vbo = 0;   // vertex buffer object
		GLuint m_vertexCount = 0; // number of vertices in vertex buffer

		// index buffer
		GLuint m_ibo = 0;   // index buffer object
		GLuint m_indexCount = 0; // number of indices index buffer
		GLenum m_indexType = 0;  // data type of index
	};
}
