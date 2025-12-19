#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Shader.h"

namespace Snail {

	class ShaderLibrary {
	public:
		static Refptr<Shader> Load(const std::string& filePath);
		static Refptr<Shader> Load(const std::string& customName, const std::string& filePath);

		static Refptr<Shader> Get(const std::string& name);
	private:
		static std::string PathsToKey(const std::string& path);
	private:
		static std::unordered_map<std::string, Refptr<Shader>> m_ShaderNames;
		static std::unordered_map<std::string, Refptr<Shader>> m_ShaderPaths;
	};

}
