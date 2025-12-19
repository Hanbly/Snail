#include "SNLpch.h"

#include "ShaderLibrary.h"
#include "TextureLibrary.h"

namespace Snail {

	std::unordered_map<std::string, Refptr<Texture>> TextureLibrary::m_TextureNames = std::unordered_map<std::string, Refptr<Texture>>();
	std::unordered_map<std::string, Refptr<Texture>> TextureLibrary::m_TexturePaths = std::unordered_map<std::string, Refptr<Texture>>();

	Refptr<Texture> TextureLibrary::Load(const std::vector<std::string>& filePath, const TextureUsage& usage)
	{
		// 从路径提取名字 (例如 assets/images/color.png -> color)
		std::filesystem::path path = filePath[0];		// 默认首个
		std::string name = path.stem().string();		// 除去扩展名的文件名

		std::string pathKey = PathsToKey(filePath);
		
		if (m_TexturePaths.find(pathKey) != m_TexturePaths.end()) { // 资源已经加载过
			if (m_TextureNames.find(name) == m_TextureNames.end()) { // 但是找不到对应命名
				m_TextureNames[name] = m_TexturePaths[pathKey]; // 把 pathKey 对应的纹理对象添加到命名映射表
			} // else 否则就是完全相同，不做操作
			return m_TexturePaths[pathKey];
		}

		if (m_TextureNames.find(name) != m_TextureNames.end()) { // 命名已经存在
			SNL_CORE_WARN("TextureLibrary: 名字 '{0}' 已被占用，但指向的是不同资源！", name); // 返回已有资源
			return m_TextureNames[name];
		}

		Refptr<Texture> texture;
		if (filePath.size() == 1) {
			texture = Texture2D::Create(filePath, usage);
		}
		if (filePath.size() > 1) {
			texture = TextureCube::Create(filePath, usage);
		}
		m_TextureNames[name] = texture;
		m_TexturePaths[pathKey] = texture;

		return texture;
	}

	Refptr<Texture> TextureLibrary::Load(const std::string& customName, const std::vector<std::string>& filePath, const TextureUsage& usage)
	{
		std::string pathKey = PathsToKey(filePath);

		if (m_TexturePaths.find(pathKey) != m_TexturePaths.end()) { // 资源已经加载过
			if (m_TextureNames.find(customName) == m_TextureNames.end()) { // 但是找不到对应命名
				m_TextureNames[customName] = m_TexturePaths[pathKey]; // 把 pathKey 对应的纹理对象添加到命名映射表
			} // else 否则就是完全相同，不做操作
			return m_TexturePaths[pathKey];
		}

		if (m_TextureNames.find(customName) != m_TextureNames.end()) { // 命名已经存在
			SNL_CORE_WARN("TextureLibrary: 名字 '{0}' 已被占用，但指向的是不同资源！", customName); // 返回已有资源
			return m_TextureNames[customName];
		}

		Refptr<Texture> texture;
		if (filePath.size() == 1) {
			texture = Texture2D::Create(filePath, usage);
		}
		if (filePath.size() > 1) {
			texture = TextureCube::Create(filePath, usage);
		}
		m_TextureNames[customName] = texture;
		m_TexturePaths[pathKey] = texture;

		return texture;
	}

	Refptr<Texture> TextureLibrary::Get(const std::string& name)
	{
		int status = m_TextureNames.find(name) != m_TextureNames.end(); // 1表示name在映射表中已经存在
		SNL_CORE_ASSERT(status, "TextureLibrary: 纹理库不存在该 texture '{0}' ", name);

		return m_TextureNames[name];
	}

	std::string TextureLibrary::PathsToKey(const std::vector<std::string>& paths) {
		if (paths.size() == 1) return paths[0];
		std::string key;
		for (const auto& p : paths) key += p + ";";
		return key;
	}

}