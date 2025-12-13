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

	struct TextureData {
		Refptr<Texture> texture;
		std::string type;
		std::string path;

		TextureData() = default;
		TextureData(const Refptr<Texture>& tex, const std::string& type)
			: texture(tex), type(type)
		{
			if (tex) {
				path = tex->GetPath();
			}
			else {
				path = "";
				SNL_CORE_ERROR("TextureData initialized with null texture! Type: {0}", type);
			}
		}
	};

	class Mesh {
	private:
		Refptr<VertexArray> m_VAO;
		Refptr<Material> m_Material;
		glm::mat4 m_LocalTransform; // 保存某部分 mesh 相对于整体 Model 原点的偏移
	public:
		glm::vec3 m_MinVertex;
		glm::vec3 m_MaxVertex;
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
			const Refptr<Shader>& shader, const std::vector<TextureData>& textures = {}, 
			const glm::mat4& localTransform = glm::mat4(1.0f));
		~Mesh() = default;

		inline const Refptr<VertexArray>& GetVAO() const { return m_VAO; }
		inline const Refptr<Material>& GetMaterial() const { return m_Material; }
		inline const glm::mat4& GetLocationTransform() const { return m_LocalTransform; }

		void Draw(const glm::mat4& worldTransform, const bool& edgeEnable) const;

	private:
		void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,const Refptr<Shader>& shader, const std::vector<TextureData>& textures = {});
		void CalculateBoundingBox(const std::vector<Vertex>& vertices);
	};

}