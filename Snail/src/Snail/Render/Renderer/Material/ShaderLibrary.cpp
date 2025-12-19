#include "SNLpch.h"

#include "ShaderLibrary.h"

namespace Snail {

	std::unordered_map<std::string, Refptr<Shader>> ShaderLibrary::m_ShaderNames = std::unordered_map<std::string, Refptr<Shader>>();
	std::unordered_map<std::string, Refptr<Shader>> ShaderLibrary::m_ShaderPaths = std::unordered_map<std::string, Refptr<Shader>>();

	Refptr<Shader> ShaderLibrary::Load(const std::string& filePath)
	{
		// 从路径提取名字 (例如 assets/shaders/Cube.glsl -> Cube)
		std::filesystem::path path = filePath;
		std::string name = path.stem().string();          // 除去扩展名的文件名

		std::string pathKey = PathsToKey(filePath);

		if (m_ShaderPaths.find(pathKey) != m_ShaderPaths.end()) { // 资源已经加载过
			if (m_ShaderNames.find(name) == m_ShaderNames.end()) { // 但是找不到对应命名
				m_ShaderNames[name] = m_ShaderPaths[pathKey]; // 把 pathKey 对应的着色器对象添加到命名映射表
			} // else 否则就是完全相同，不做操作
			return m_ShaderPaths[pathKey];
		}

		if (m_ShaderNames.find(name) != m_ShaderNames.end()) { // 命名已经存在
			SNL_CORE_WARN("ShaderLibrary: 名字 '{0}' 已被占用，但指向的是不同资源！", name); // 返回已有资源
			return m_ShaderNames[name];
		}

		const Refptr<Shader>& shader = Shader::Create(filePath);
		m_ShaderNames[name] = shader;
		m_ShaderPaths[pathKey] = shader;

		return shader;
	}

	Refptr<Shader> ShaderLibrary::Load(const std::string& customName, const std::string& filePath)
	{
		std::string pathKey = PathsToKey(filePath);

		if (m_ShaderPaths.find(pathKey) != m_ShaderPaths.end()) { // 资源已经加载过
			if (m_ShaderNames.find(customName) == m_ShaderNames.end()) { // 但是找不到对应命名
				m_ShaderNames[customName] = m_ShaderPaths[pathKey]; // 把 pathKey 对应的着色器对象添加到命名映射表
			} // else 否则就是完全相同，不做操作
			return m_ShaderPaths[pathKey];
		}

		if (m_ShaderNames.find(customName) != m_ShaderNames.end()) { // 命名已经存在
			SNL_CORE_WARN("ShaderLibrary: 名字 '{0}' 已被占用，但指向的是不同资源！", customName); // 返回已有资源
			return m_ShaderNames[customName];
		}

		const Refptr<Shader>& shader = Shader::Create(customName, filePath);
		m_ShaderNames[customName] = shader;
		m_ShaderPaths[pathKey] = shader;

		return shader;
	}

	Refptr<Shader> ShaderLibrary::Get(const std::string& name)
	{
		int status = m_ShaderNames.find(name) != m_ShaderNames.end(); // 1表示name在映射表中已经存在
		SNL_CORE_ASSERT(status, "ShaderLibrary: 着色器库不存在该 shader '{0}' ", name);

		return m_ShaderNames[name];
	}

	std::string ShaderLibrary::PathsToKey(const std::string& path) {
		return path;
	}

}