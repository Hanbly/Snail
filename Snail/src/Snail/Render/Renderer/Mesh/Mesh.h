#pragma once

#include "glm/glm.hpp"

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

#include "Snail/Render/RenderAPI/VertexArray.h"
#include "Snail/Render/Renderer/Material/Material.h"

namespace Snail {

	struct TextureData {
		Refptr<Texture> texture;
		std::string type;
		std::string path;

		TextureData(const Refptr<Texture>& tex, const std::string& type)
			: texture(tex), type(type)
		{
			path = tex->GetPath();
		}
	};

	class Mesh {
	private:
		Refptr<VertexArray> m_VAO;
		Refptr<Material> m_Material;
		glm::mat4 m_LocalTransform; // 保存某部分 mesh 相对于整体 Model 原点的偏移
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
			const Refptr<Shader>& shader, const glm::mat4& localTransform = glm::mat4(1.0f));
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
			const std::vector<TextureData>& textures, const Refptr<Shader>& shader,
			const glm::mat4& localTransform = glm::mat4(1.0f));
		~Mesh() = default;

		inline const Refptr<Material>& GetMaterial() const { return m_Material; }

		void Draw(const glm::mat4& worldTransform) const;

	private:
		void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Refptr<Shader>& shader);
		void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, 
			const std::vector<TextureData>& textures, const Refptr<Shader>& shader);
	};

}