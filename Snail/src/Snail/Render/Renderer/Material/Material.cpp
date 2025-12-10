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

		int maxTextureUnits;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

		for (const auto& [name, texture] : m_Textures)
		{
			if (slot >= maxTextureUnits) {
				SNL_CORE_WARN("Material Bind: 纹理数量超过上限 ({0})! 跳过加载 {1}", maxTextureUnits, name);
				break;
			}

			texture->Bind(slot);
			m_Shader->SetInt(name, slot);
			slot++;
		}
	}

}
