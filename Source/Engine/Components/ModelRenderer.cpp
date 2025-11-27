#include "EnginePCH.h"
#include "ModelRenderer.h"

namespace neu {
	FACTORY_REGISTER(ModelRenderer)
void ModelRenderer::Update(float dt)
{
}

void ModelRenderer::Draw(Renderer& renderer)
{
	if (material) {

	material->Bind();
	material->program->SetUniform("u_model", owner->transform.GetMatrix());
	}

	glDepthMask(enableDepth);
	glCullFace(cullFace);

	if (model) {
	model->Draw(GL_TRIANGLES);

	}
}

void ModelRenderer::Draw(Renderer& renderer, Program& program) {
	program.SetUniform("u_model", owner->transform.GetMatrix());
	
	glDepthMask(enableDepth);
	glCullFace(cullFace);

	if (model) {
		model->Draw(GL_TRIANGLES);

	}
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
	std::string text;
	
	text = (model) ? model->name : "none";
	ImGui::Text("Model: %s", text.c_str());
	Editor::GetDialogResource<Model>(model, "ModelDialog", "Open model", "Model file (*.obj;*.fbx;*.glb){.obj,.fbx,.glb},.*");

	text = (model) ? material->name : "none";
	ImGui::Text("Material: %s", text.c_str());
	Editor::GetDialogResource<Material>(material, "MaterialDialog", "Open material", "Material file (*.mat){.mat},.*");

}

}

