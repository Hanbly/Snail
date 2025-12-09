#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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

    struct ModelInstance {
        std::string name;       // 在编辑器里显示的名字
        Refptr<Model> model;    // 指向的模型资源
        Refptr<Mesh> mesh;      // 兼容单一 Mesh

        // 变换参数
        glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
        glm::vec3 rot = { 0.0f, 0.0f, 0.0f };
        glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

        // 材质参数 (每个物体独立)
        float ambient = 0.1f;
        float diffuse = 0.8f;
        float specular = 0.5f;
        float shininess = 32.0f;

        // 获取变换矩阵
        glm::mat4 GetTransform() const {
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, pos);
            transform = glm::rotate(transform, glm::radians(rot.x), { 1,0,0 });
            transform = glm::rotate(transform, glm::radians(rot.y), { 0,1,0 });
            transform = glm::rotate(transform, glm::radians(rot.z), { 0,0,1 });
            transform = glm::scale(transform, scale);
            return transform;
        }
    };

}