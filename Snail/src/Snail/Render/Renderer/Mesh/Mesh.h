#pragma once

#include "glm/glm.hpp"

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/RenderAPI/VertexArray.h"
#include "Snail/Render/Renderer/Material/Material.h"

namespace Snail {

	struct AABB {
		glm::vec3 min;
		glm::vec3 max;
	};

	enum class PrimitiveType {
		None = 0, // 非图元
		Cube, Sphere, Plane, Skybox
	};

	static std::string PrimitiveTypeToString(const PrimitiveType& type)
	{
		switch (type) {
			case PrimitiveType::Cube: return "Cube";
			case PrimitiveType::Sphere: return "Sphere";
			case PrimitiveType::Plane: return "Plane";
			case PrimitiveType::Skybox: return "Skybox";
			default: return "NonePrimitive";
		}
	}

	static PrimitiveType StringToPrimitiveType(const std::string& typestr)
	{
		if (typestr == "Cube") { return PrimitiveType::Cube; }
		if (typestr == "Sphere") { return PrimitiveType::Sphere; }
		if (typestr == "Plane") { return PrimitiveType::Plane; }
		if (typestr == "Skybox") { return PrimitiveType::Skybox; }
		if (typestr == "None") { return PrimitiveType::None; }
		SNL_CORE_ASSERT(false, "Mesh::StringToPrimitiveType: 未知图元类型! {0}", typestr.c_str());

		return PrimitiveType::None;
	}

	class Mesh {
	private:
		Refptr<VertexArray> m_VAO;
		Refptr<Material> m_Material;
		glm::mat4 m_LocalTransform; // 保存某部分 mesh 相对于整体 Model 原点的偏移

		PrimitiveType m_PrimitiveType = PrimitiveType::None;
	public:
		glm::vec3 m_MinVertex;
		glm::vec3 m_MaxVertex;
	public:
		Mesh(const PrimitiveType& type, 
			const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
			const Refptr<Shader>& shader, const std::vector<Refptr<Texture>>& textures = {}, 
			const glm::mat4& localTransform = glm::mat4(1.0f));
		~Mesh() = default;

		// ========================= Get方法 ================================
		inline const Refptr<VertexArray>& GetVAO() const { return m_VAO; }
		inline const Refptr<Material>& GetMaterial() const { return m_Material; }
		inline const glm::mat4& GetLocationTransform() const { return m_LocalTransform; }
		inline const PrimitiveType& GetPrimitiveType() const { return m_PrimitiveType; }
		// shader 文件路径
		inline const std::string GetShaderPath() const							{ return GetMaterial()->GetShader()->GetFilePath(); }
		// Texture2D | TextureCube | ...
		inline std::vector<TextureType> GetTexturesDimensionsType() const		{ return GetMaterial()->GetTexturesDimensionsType(); }
		// diffuse | specular | ...
		inline std::vector<std::string> GetTexturesUsageType() const			{ return GetMaterial()->GetTexturesUsageType(); }
		// 全部纹理 文件路径
		inline std::vector<std::vector<std::string>> GetTexturesAssets() const { return GetMaterial()->GetTexturesAssets(); }
		// 全部纹理
		inline std::vector<Refptr<Texture>> GetTextures() const { return GetMaterial()->GetTextures(); }
		// 可选 非assimp加载的 mesh 数据
		inline const std::vector<Vertex> GetVertices() const					{ return GetVAO()->GetVertexBuffer(); }
		inline const std::vector<uint32_t> GetIndices() const					{ return GetVAO()->GetIndexBuffer()->GetIndices(); }

		// --- 修改着色器 ---
		void EditShader(const std::string& path) const;
		// --- 添加纹理 ---
		void AddTexture(const Refptr<Texture>& texture, const TextureUsage& usage);
		// --- 修改纹理 ---
		void EditTexture(size_t index, const std::vector<std::string>& assetPath) const;
		// --- 删除纹理 ---
		void RemoveTexture(size_t index);
		// --- 清空纹理 ---
		void ClearTextures();

		// --- 绘制 ---
		void Draw(const glm::mat4& worldTransform, const bool& edgeEnable) const;

	private:
		void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,const Refptr<Shader>& shader, const std::vector<Refptr<Texture>>& textures = {});
		void CalculateBoundingBox(const std::vector<Vertex>& vertices);
		void RemapMaterialTextures(const std::vector<Refptr<Texture>>& textures);
	};

}