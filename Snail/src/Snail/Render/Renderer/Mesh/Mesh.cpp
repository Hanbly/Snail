#include "SNLpch.h"

#include "Snail/Render/Renderer/Material/ShaderLibrary.h"
#include "Snail/Render/Renderer/Material/TextureLibrary.h"
#include "Snail/Render/Renderer/Renderer3D.h"

#include "Mesh.h"

namespace Snail {

	Mesh::Mesh(const PrimitiveType& type, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Refptr<Shader>& shader, const std::vector<Refptr<Texture>>& textures, const glm::mat4& localTransform)
		: m_PrimitiveType(type), m_LocalTransform(localTransform)
	{
		SNL_PROFILE_FUNCTION();


		SetupMesh(vertices, indices, shader, textures);
	}

	void Mesh::EditShader(const std::string& path) const
	{
		auto newShader = ShaderLibrary::Load(path, {});
		GetMaterial()->SetShader(newShader);
	}

	void Mesh::AddTexture(const Refptr<Texture>& texture, const TextureUsage& usage)
	{
		// --------------------- 获取原有材质 + 添加新的材质 -------------------------------
		auto textures = GetTextures();
		textures.push_back(texture);

		// 重新映射材质的纹理缓存
		RemapMaterialTextures(textures);
	}

	void Mesh::EditTexture(size_t index, const std::vector<std::string>& assetPath) const
	{
		auto textures = GetTextures(); // 获取当前纹理列表

		TextureUsage usage = textures[index]->GetUsage(); // 保持原有的 Usage
		auto newTexture = TextureLibrary::Load(assetPath, usage);

		auto& toEdit = textures[index];

		if (index < textures.size()) {
			std::string uniformName = GetMaterial()->GetTextureUniformName(toEdit);
			GetMaterial()->SetTexture(uniformName, newTexture);
		}
	}

	void Mesh::RemoveTexture(size_t index)
	{
		// --------------------- 获取原有材质 + 删除对应的材质 -------------------------------
		auto textures = GetTextures();
		if (index >= textures.size()) return;

		//TextureLibrary::Remove(textures[index]);
		textures.erase(textures.begin() + index);

		// 重新映射材质的纹理缓存
		RemapMaterialTextures(textures);
	}

	void Mesh::ClearTextures()
	{
		auto textures = GetTextures();
		textures.clear();

		// 重新映射材质的纹理缓存
		RemapMaterialTextures(textures);
	}

	void Mesh::Draw(const glm::mat4& worldTransform, const bool& edgeEnable) const
	{
		SNL_PROFILE_FUNCTION();


		Renderer3D::DrawMesh(*this, edgeEnable, worldTransform * m_LocalTransform);
	}

	void Mesh::SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Refptr<Shader>& shader, const std::vector<Refptr<Texture>>& textures)
	{
		SNL_PROFILE_FUNCTION();

		

		// ----------------------初始化vao-------------------------------
		m_VAO = VertexArray::Create();
		m_VAO->Bind();

		uint32_t dataSize = static_cast<uint32_t>(vertices.size() * sizeof(Vertex));
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
		m_VAO->SetVertexBuffer(vbo);

		dataSize = static_cast<uint32_t>(indices.size() * sizeof(uint32_t));
		Refptr<IndexBuffer> ibo = IndexBuffer::Create(indices.data(), dataSize);
		m_VAO->SetIndexBuffer(ibo);

		// ---------------------初始化材质-------------------------------
		m_Material = Snail::Material::Create(shader);
		RemapMaterialTextures(textures); // 映射纹理的同一变量名称

		// 暂时用默认值代替
		//m_Material->SetFloat("u_AmbientStrength", 0.1f);  // 默认一点点环境光 // 现在依据Scene类的属性来设置了
		//m_Material->SetFloat("u_DiffuseStrength", 0.8f);  // 默认较强的漫反射
		//m_Material->SetFloat("u_SpecularStrength", 0.5f); // 默认中等高光
		m_Material->SetFloat("u_Shininess", 32.0f);       // 默认反光度

		CalculateBoundingBox(vertices);
	}

	void Mesh::CalculateBoundingBox(const std::vector<Vertex>& vertices)
	{
		if (vertices.empty()) return;

		m_MinVertex = vertices[0].position;
		m_MaxVertex = vertices[0].position;

		for (const auto& v : vertices) {
			m_MinVertex.x = std::min(m_MinVertex.x, v.position.x);
			m_MinVertex.y = std::min(m_MinVertex.y, v.position.y);
			m_MinVertex.z = std::min(m_MinVertex.z, v.position.z);

			m_MaxVertex.x = std::max(m_MaxVertex.x, v.position.x);
			m_MaxVertex.y = std::max(m_MaxVertex.y, v.position.y);
			m_MaxVertex.z = std::max(m_MaxVertex.z, v.position.z);
		}
	}

	void Mesh::RemapMaterialTextures(const std::vector<Refptr<Texture>>& textures)
	{
		GetMaterial()->ClearTexture();

		if (textures.size()) {
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int cubemapNr = 1;

			for (const auto& texData : textures) {
				std::string number;
				std::string usage = TextureUsageToString(texData->GetUsage()); // e.g. "texture_diffuse"

				if (usage == "texture_normal")
					m_Material->SetInt("u_UseTextureNormal", 1);
				else
					m_Material->SetInt("u_UseTextureNormal", 0);

				if (usage == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (usage == "texture_specular")
					number = std::to_string(specularNr++);
				else if (usage == "texture_normal")
					number = std::to_string(normalNr++);
				else if (usage == "texture_cubemap")
					number = std::to_string(cubemapNr++);

				// 将 "texture_diffuse" 映射为 "u_Diffuse" + "1"
				std::string uniformName;
				if (usage == "texture_diffuse")			uniformName = "u_Diffuse" + number;
				else if (usage == "texture_specular")	uniformName = "u_Specular" + number;
				else if (usage == "texture_normal")	uniformName = "u_Normal" + number;
				else if (usage == "texture_cubemap")		uniformName = "u_Cubemap" + number;

				// 将纹理对象传递给 Material
				m_Material->SetTexture(uniformName, texData);
			}
			m_Material->SetInt("u_UseTexture", 1);
		}
		else if (textures.size() == 0) {
			m_Material->SetInt("u_UseTexture", 0);
		}
	}

}