#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Shader.h"

namespace Snail {

	class ShaderLibrary {
	public:
		static Refptr<Shader> Load(const std::string& rawPath, const std::vector<std::string>& macros);
		static Refptr<Shader> Load(const std::string& customName, const std::string& rawPath, const std::vector<std::string>& macros);

		static Refptr<Shader> Get(const std::string& name);

		static std::string PathsMacrosToKey(const std::string& path, const std::vector<std::string>& macros);
	private:
		static void CleanFilePath(std::string& rawPath);
	private:
		static std::unordered_map<std::string, Refptr<Shader>> m_ShaderNames;
		static std::unordered_map<std::string, Refptr<Shader>> m_ShaderPaths;
		static std::unordered_map<std::string, Refptr<Shader>> m_ShaderMacros;
	};

}
