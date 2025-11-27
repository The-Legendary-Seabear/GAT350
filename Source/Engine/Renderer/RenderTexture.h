#pragma once
#include "Texture.h"

namespace neu {
	class RenderTexture : public Texture {
	public:
		~RenderTexture();

		bool Create(int width, int height, bool depth = true);
		bool CreateDepth(int width, int height);
		bool Load(const std::string& filename);

		void BindFramebuffer();
		void UnbindFramebuffer();

	public:
		GLuint m_fbo = 0;
		GLuint m_depthBuffer = 0;
	};
}
