#include "SNLpch.h"

#include "Snail/Render/Renderer/Renderer3D.h"

#include "Mesh.h"

namespace Snail {

	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Refptr<Shader>& shader, const glm::mat4& localTransform)
		: m_LocalTransform(localTransform)
	{
		SetupMesh(vertices, indices, shader);
	}

	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<TextureData>& textures, const Refptr<Shader>& shader, const glm::mat4& localTransform)
		:  m_LocalTransform(localTransform)
	{
		SetupMesh(vertices, indices, textures, shader);
	}

	void Mesh::Draw(const glm::mat4& worldTransform) const
	{
		Renderer3D::DrawMesh(m_VAO, m_Material, worldTransform);
	}

	void Mesh::SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Refptr<Shader>& shader)
	{
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

		m_Material->SetFloat("u_Shininess", 32.0f);
	}

	void Mesh::SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::vector<TextureData>& textures, const Refptr<Shader>& shader)
	{
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

		// 设置默认材质属性
		// 注意：这些变量 (u_AmbientStrength) 需要作为 Mesh 的成员变量存储，或者通过构造函数传入
		// 暂时用默认值代替
		m_Material->SetFloat("u_Shininess", 32.0f);
	}

}