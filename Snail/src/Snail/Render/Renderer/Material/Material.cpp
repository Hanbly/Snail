#include "SNLpch.h"

#include "Material.h"

namespace Snail {

	void Material::Bind() const
	{
		m_Shader->Bind();

		// 上传所有缓存的 Uniform 数据
		for (const auto& [name, value] : m_Ints) m_Shader->SetInt(name, value);
		for (const auto& [name, value] : m_Floats) m_Shader->SetFloat(name, value);
		for (const auto& [name, value] : m_Float3s) m_Shader->SetFloat3(name, value);
		for (const auto& [name, value] : m_Float4s) m_Shader->SetFloat4(name, value);
		for (const auto& [name, value] : m_Mat3s) m_Shader->SetMat3(name, value);
		for (const auto& [name, value] : m_Mat4s) m_Shader->SetMat4(name, value);

		// 绑定纹理
		int slot = 0;
		for (const auto& [name, texture] : m_Textures)
		{
			if (texture) {
				texture->Bind(slot);
				m_Shader->SetInt(name, slot);
				slot++;
			}
			else {
				SNL_CORE_WARN("Material: 尝试绑定名为 '{0}' 的空纹理!", name);
			}
		}
	}

	std::vector<TextureType> Material::GetTexturesDimensionsType() const
	{
		std::vector<TextureType> dimensionsTypes;
		dimensionsTypes.reserve(m_Textures.size());
		for (auto& tex : m_Textures) {
			dimensionsTypes.push_back(tex.second->GetType());
		}
		return dimensionsTypes;
	}

	std::vector<std::string> Material::GetTexturesUsageType() const
	{
		std::vector<std::string> usageTypes;
		usageTypes.reserve(m_Textures.size());
		for (auto& tex : m_Textures) {
			usageTypes.push_back(TextureUsageToString(tex.second->GetUsage()));
		}
		return usageTypes;
	}

	std::vector<std::vector<std::string>> Material::GetTexturesAssets() const
	{
		std::vector<std::vector<std::string>> assetsList;
		assetsList.reserve(m_Textures.size());
		for (auto& tex : m_Textures) {
			int assetsCount = static_cast<int>(tex.second->GetPath().size());
			std::vector<std::string> assets;
			if (assetsCount == 1) {
				assets.push_back(tex.second->GetPath()[0]);
			}
			else if (assetsCount > 1) {
				for(int i = 0; i < assetsCount; ++i) assets.push_back(tex.second->GetPath()[i]);
			}
			else {
				SNL_CORE_ASSERT(false, "Material::GetTexturesAssets(): 纹理资源数量错误! count: {0}", assetsCount);
			}
			assetsList.push_back(assets);
		}
		return assetsList;
	}

	std::vector<Refptr<Texture>> Material::GetTextures() const
	{
		std::vector<Refptr<Texture>> textures;
		textures.reserve(m_Textures.size());
		for (auto& tex : m_Textures) {
			textures.push_back(tex.second);
		}
		return textures;
	}

	std::string Material::GetTextureUniformName(const Refptr<Texture>& texture)
	{
		for (auto& tex : m_Textures) {
			if (tex.second == texture) {
				return tex.first;
			}
		}
		return std::string();
	}

}
