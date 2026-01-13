#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Shader.h"
#include "Texture.h"

namespace Snail {

	class Material
	{
	public:
		Material(const Refptr<Shader>& shader)
			: m_Shader(shader) {}
		~Material() = default;

		// 绑定 Shader 并上传材质所有的特有参数
		void Bind() const;
		void BindToShader(const Refptr<Shader>& shader);

		void SetShader(const Refptr<Shader>& shader) { m_Shader = shader; }
		Refptr<Shader> GetShader() const { return m_Shader; }

		std::vector<TextureType> GetTexturesDimensionsType() const; // Texture2D | TextureCube | ...
		std::vector<std::string> GetTexturesUsageType() const;		// diffuse | specular | ...
		std::vector<std::vector<std::string>> GetTexturesAssets() const;	// 文件路径
		std::vector<Refptr<Texture>> GetTextures() const;

		// --- 参数设置接口 ---
		void SetInt(const std::string& name, int value) { m_Ints[name] = value; }
		void SetFloat(const std::string& name, float value) { m_Floats[name] = value; }
		void SetFloat3(const std::string& name, const glm::vec3& value) { m_Float3s[name] = value; }
		void SetFloat4(const std::string& name, const glm::vec4& value) { m_Float4s[name] = value; }
		void SetMat3(const std::string& name, const glm::mat3& matrix) { m_Mat3s[name] = matrix; }
		void SetMat4(const std::string& name, const glm::mat4& matrix) { m_Mat4s[name] = matrix; }

		// 纹理设置：需要指定 Shader 中的 sampler 名字和对应的纹理资源
		void SetTexture(const std::string& name, const Refptr<Texture>& texture) { m_Textures[name] = texture; }
		std::string GetTextureUniformName(const Refptr<Texture>& texture);
		// 清空纹理（用于添加新纹理）
		void ClearTexture() { m_Textures.clear(); }

		// --- 基础材质参数 ---
		void SetAmbientColor(const glm::vec3& color) { m_AmbientColor = color; }
		glm::vec3 GetAmbientColor() const { return m_AmbientColor; }
		void SetDiffuseColor(const glm::vec3& color) { m_DiffuseColor = color; }
		glm::vec3 GetDiffuseColor() const { return m_DiffuseColor; }
		void SetSpecularColor(const glm::vec3& color) { m_SpecularColor = color; }
		glm::vec3 GetSpecularColor() const { return m_SpecularColor; }
		float GetShininess() const { return m_Shininess; }
		void SetShininess(float val) { m_Shininess = val; }
		void SetAlbedoColor(const glm::vec3& color) { m_AlbedoColor = color; }
		glm::vec3 GetAlbedoColor() const { return m_AlbedoColor; }
		void SetMetallic(float val) { m_Metallic = val; }
		float GetMetallic() const { return m_Metallic; }
		void SetRoughness(float val) { m_Roughness = val; }
		float GetRoughness() const { return m_Roughness; }
		void SetAO(float val) { m_AO = val; }
		float GetAO() const { return m_AO; }
		void SetBasicValues();

		static Refptr<Material> Create(const Refptr<Shader>& shader) { return std::make_shared<Material>(shader); }

	private:
		Refptr<Shader> m_Shader;

		// 暂存特有材质参数，在 Bind 时统一设置
		std::unordered_map<std::string, int> m_Ints;
		std::unordered_map<std::string, float> m_Floats;
		std::unordered_map<std::string, glm::vec3> m_Float3s;
		std::unordered_map<std::string, glm::vec4> m_Float4s;
		std::unordered_map<std::string, glm::mat3> m_Mat3s;
		std::unordered_map<std::string, glm::mat4> m_Mat4s;
		// uniformName + texture引用
		std::unordered_map<std::string, Refptr<Texture>> m_Textures;

		// --- 无纹理时的材质参数 ---
		glm::vec3 m_AmbientColor = glm::vec3(1.0f);		// 默认为白色
		glm::vec3 m_DiffuseColor = glm::vec3(1.0f);		// 默认为白色
		glm::vec3 m_SpecularColor = glm::vec3(1.0f);	// 默认为白色
		float m_Shininess = 32.0f;

		glm::vec3 m_AlbedoColor = glm::vec3(1.0f);	// 默认为白色
		float m_Metallic = 0.0f;					// 默认为非金属
		float m_Roughness = 0.5f;					// 默认为中等粗糙
		float m_AO = 1.0f;							// 默认为 1.0 (无遮蔽)
	};

}