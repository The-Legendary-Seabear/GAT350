#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
namespace neu {
	bool Model::Load(const std::string& filename) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			LOG_WARNING("Could not load assimp file {} : {}", filename, importer.GetErrorString());
			return false;
		}
		ProcessNode(scene->mRootNode, scene);
		return true;
	}
	void Model::Draw(GLenum primitiveType) {
		for (auto& vertexBuffer : m_vertexBuffers) {
			vertexBuffer->Draw(primitiveType);
		}
	}
	void Model::ProcessNode(aiNode* node, const aiScene* scene) {
		// process the current node meshes
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMesh(mesh, scene);
		}
		// process the current node children
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene);
		}
	}
	void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		// get model vertex attributes
		for (size_t i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			vertex.position = glm::vec3{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh -> mVertices[i].z };
			vertex.texcoord = (mesh->mTextureCoords[0]) ? glm::vec2{ mesh->mTextureCoords[0][i].x, mesh -> mTextureCoords[0][i].y } : glm::vec2{ 0, 0 };
			vertex.normal = (mesh->mNormals)
				? glm::vec3{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh -> mNormals[i].z }
			: glm::vec3{ 0, 0, 0 };
			vertex.tangent = (mesh->mTangents)
				? glm::vec3{ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh -> mTangents[i].z }
			: glm::vec3{ 0, 0, 0 };
			vertices.push_back(vertex);
		}
		// create vertex buffer and attributes
		auto vertexBuffer = std::make_shared<VertexBuffer>();
		vertexBuffer -> CreateVertexBuffer((GLsizei)(sizeof(Vertex) * vertices.size()), (GLsizei)vertices.size(), vertices.data());
vertexBuffer->SetAttribute(0, 3, sizeof(Vertex), offsetof(Vertex, position));
vertexBuffer->SetAttribute(1, 2, sizeof(Vertex), offsetof(Vertex, texcoord));
vertexBuffer->SetAttribute(2, 3, sizeof(Vertex), offsetof(Vertex, normal));
vertexBuffer->SetAttribute(3, 3, sizeof(Vertex), offsetof(Vertex, tangent));
// get model index vertices
std::vector<GLuint> indices;
for (size_t i = 0; i < mesh->mNumFaces; i++)
{
	aiFace face = mesh->mFaces[i];
	for (size_t j = 0; j < face.mNumIndices; j++)
	{
		indices.push_back(face.mIndices[j]);
	}
}
// create index vertex buffer
vertexBuffer->CreateIndexBuffer(GL_UNSIGNED_INT, (GLsizei)indices.size(), indices.data());
// add vertex buffer to vertex buffers
m_vertexBuffers.push_back(vertexBuffer);
	}
}
