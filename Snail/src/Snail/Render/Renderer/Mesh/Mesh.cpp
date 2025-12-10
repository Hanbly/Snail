#include "SNLpch.h"

#include "Snail/Render/Renderer/Renderer3D.h"

#include "Mesh.h"

namespace Snail {

	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Refptr<Shader>& shader, const std::vector<TextureData>& textures, const glm::mat4& localTransform)
		:  m_LocalTransform(localTransform)
	{
		SNL_PROFILE_FUNCTION();


		SetupMesh(vertices, indices, shader, textures);
	}

	void Mesh::Draw(const glm::mat4& worldTransform) const
	{
		SNL_PROFILE_FUNCTION();


		Renderer3D::DrawMesh(m_VAO, m_Material, worldTransform * m_LocalTransform);
	}

	void Mesh::SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Refptr<Shader>& shader, const std::vector<TextureData>& textures)
	{
		SNL_PROFILE_FUNCTION();

		

		// ----------------------初始化vao-------------------------------
		m_VAO = VertexArray::Create();
		m_VAO->Bind();

		uint32_t dataSize = vertices.size() * sizeof(Vertex);
		Refptr<VertexBuffer> vbo = VertexBuffer::Create(vertices.data(), dataSize);
		vbo->Bind();

		// 创建 & 启用布局layout
		Refptr<BufferLayout> layout = BufferLayout::Create(
			{
				{ "a_Position",  VertexDataType::Float3 },
				{ "a_Normal",    VertexDataType::Float3 },
				{ "a_TexCoord",  VertexDataType::Float2 }
			}
		);
		vbo->SetLayout(layout);
		m_VAO->AddVertexBuffer(vbo);

		dataSize = indices.size() * sizeof(uint32_t);
		Refptr<IndexBuffer> ibo = IndexBuffer::Create(indices.data(), dataSize);
		m_VAO->SetIndexBuffer(ibo);

		// ---------------------初始化材质-------------------------------
		m_Material = Snail::Material::Create(shader);
		if (textures.size()) {

			// 计数器，用于处理同类型的多张纹理 (diffuse1, diffuse2...)
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;

			for (const auto& texData : textures) {
				std::string number;
				std::string name = texData.type; // e.g. "texture_diffuse"

				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);

				// 拼接 Shader 中的 Uniform 名字
				// 假设 Shader 里写的是: uniform sampler2D texture_diffuse1;
				// 或者是你自定义的名字: u_Diffuse1
				// 这里需要和你的 Shader 命名约定一致。
				// 示例：将 "texture_diffuse" 映射为 "u_Diffuse" + "1"
				std::string uniformName;
				if (name == "texture_diffuse") uniformName = "u_Diffuse" + number;
				else if (name == "texture_specular") uniformName = "u_Specular" + number;

				// 将纹理对象传递给 Material
				m_Material->SetTexture(uniformName, texData.texture);
			}
			m_Material->SetInt("u_UseTexture", 1);
		}
		else if (textures.size() == 0) {
			m_Material->SetInt("u_UseTexture", 0);
		}

		// 暂时用默认值代替
		m_Material->SetFloat("u_AmbientStrength", 0.1f);  // 默认一点点环境光
		m_Material->SetFloat("u_DiffuseStrength", 0.8f);  // 默认较强的漫反射
		m_Material->SetFloat("u_SpecularStrength", 0.5f); // 默认中等高光
		m_Material->SetFloat("u_Shininess", 32.0f);       // 默认反光度
	}

}