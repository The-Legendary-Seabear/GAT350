#include "EnginePCH.h"
#include "ModelRenderer.h"

namespace neu {
	FACTORY_REGISTER(ModelRenderer)
void ModelRenderer::Update(float dt)
{
}

void ModelRenderer::Draw(Renderer& renderer)
{
	material->Bind();
	material->program->SetUniform("u_model", owner->transform.GetMatrix());

	glDepthMask(enableDepth);
	glCullFace(cullFace);

	model->Draw(GL_TRIANGLES);
}

void ModelRenderer::Read(const serial_data_t& value) {
	Object::Read(value);

	std::string modelName;
	SERIAL_READ_NAME(value, "model", modelName);

	model = Resources().Get<Model>(modelName);

	std::string materialName;
	SERIAL_READ_NAME(value, "material", materialName);

	material = Resources().Get<Material>(materialName);

	SERIAL_READ(value, enableDepth);

	std::string cullFaceName;
	SERIAL_READ_NAME(value, "cullFace", cullFaceName);
	if (equalsIgnoreCase(cullFaceName, "front")) {
		cullFace = GL_FRONT;
	}
}

void ModelRenderer::UpdateGui()
{
}

}

