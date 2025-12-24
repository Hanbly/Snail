#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/Renderer/Mesh/Mesh.h"
#include "Snail/Render/Renderer/Material/TextureLibrary.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#pragma warning(push)
#pragma warning(disable : 4244)// 禁用 C4244 警告 (从 'type1' 转换到 'type2'，可能丢失数据)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma warning(pop)// 恢复之前的警告状态

namespace Snail {

	class Model {
	private:
		Refptr<Shader> m_Shader;
		std::vector<Refptr<Mesh>> m_Meshes;
		std::string m_FullPath;
		std::string m_Directory;

		bool m_IsImported = false;
		PrimitiveType m_PrimitiveType = PrimitiveType::None;
	public:
		AABB m_AABB;
	public:
		// 单个mesh构造
		Model(const Refptr<Mesh>& mesh);
		// 单个mesh构造 注意传入图元类型 自动依据 type 生成数据
		Model(const PrimitiveType& type,
			const Refptr<Shader>& shader, const std::vector<Refptr<Texture>>& textures = {},
			const glm::mat4& localTransform = glm::mat4(1.0f));
		// 单个mesh构造 注意传入图元类型
		Model(const PrimitiveType& type,
			const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
			const Refptr<Shader>& shader, const std::vector<Refptr<Texture>>& textures = {},
			const glm::mat4& localTransform = glm::mat4(1.0f));
		// 多mesh构造 外部导入模型
		Model(const Refptr<Shader>& shader, const std::string& objPath);
		~Model() = default;

		inline const std::string& GetShaderPath() const { return m_Shader->GetFilePath(); }
		inline const std::vector<Refptr<Mesh>>& GetMeshes() const { return m_Meshes; }
		inline const std::string& GetFullPath() const { return m_FullPath; }
		inline bool IsImported() const { return m_IsImported; }
		inline const PrimitiveType& GetPrimitiveType() const { return m_PrimitiveType; }

		void Draw(const glm::mat4& worldTransform, const bool& edgeEnable) const;

	private:
		void Load(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransformation);
		Refptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& localTransformation);
		std::vector<Refptr<Texture>> LoadMaterialTextures(aiMaterial* mat, const aiTextureType& type, const TextureUsage& usage);

		//--------------Tools--------------------
		glm::mat4 ConvertaiMat4ToglmMat4(const aiMatrix4x4& matrix) const;
		void ConsiderMeshAABB(const Refptr<Mesh>& mesh);
		std::pair<std::vector<Vertex>, std::vector<uint32_t>> GetPrimitiveDatas(const PrimitiveType& type);
	};

}