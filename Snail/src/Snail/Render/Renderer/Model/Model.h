#pragma once

#include "glm/glm.hpp"

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

#include "Snail/Render/Renderer/Mesh/Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace Snail {

	class Model {
	private:
		Refptr<Shader> m_Shader;
		std::vector<Refptr<Mesh>> m_Meshs;
		std::vector<TextureData> m_LoadedTextures;
		std::string m_Directory;
	public:
		Model(const Refptr<Shader>& shader, const std::string& path);
		~Model() = default;

		inline const std::vector<Refptr<Mesh>>& GetMeshs() const { return m_Meshs; }

		void Draw(const glm::mat4& worldTransform) const;

	private:
		void Load(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransformation);
		Refptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& localTransformation);
		std::vector<TextureData> LoadMaterialTextures(aiMaterial* mat, const aiTextureType& type, const std::string& typeName);

		//--------------Tools--------------------
		glm::mat4 ConvertaiMat4ToglmMat4(const aiMatrix4x4& matrix) const;
	};

}