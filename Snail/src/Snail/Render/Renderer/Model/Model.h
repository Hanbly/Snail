#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/Renderer/Mesh/Mesh.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace Snail {

	class Model {
	private:
		Refptr<Shader> m_Shader;
		std::vector<Refptr<Mesh>> m_Meshes;
		std::vector<TextureData> m_LoadedTextures;
		std::string m_Directory;
	public:
		AABB m_AABB;
	public:
		Model(const Refptr<Mesh>& mesh);
		Model(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
			const Refptr<Shader>& shader, const std::vector<TextureData>& textures = {},
			const glm::mat4& localTransform = glm::mat4(1.0f));

		Model(const Refptr<Shader>& shader, const std::string& path);
		~Model() = default;

		inline const std::vector<Refptr<Mesh>>& GetMeshs() const { return m_Meshes; }

		void Draw(const glm::mat4& worldTransform, const bool& edgeEnable) const;

	private:
		void Load(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransformation);
		Refptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& localTransformation);
		std::vector<TextureData> LoadMaterialTextures(aiMaterial* mat, const aiTextureType& type, const std::string& typeName);

		//--------------Tools--------------------
		glm::mat4 ConvertaiMat4ToglmMat4(const aiMatrix4x4& matrix) const;
		void ConsiderMeshAABB(const Refptr<Mesh>& mesh);
	};

}