#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Texture.h"

namespace Snail {

	class TextureLibrary {
	public:
		static Refptr<Texture> Load(const std::vector<std::string>& filePath, const TextureUsage& usage);
		static Refptr<Texture> Load(const std::string& customName, const std::vector<std::string>& filePath, const TextureUsage& usage);

		static Refptr<Texture> Get(const std::string& name);

		static void Remove(const std::string& name);
		static void Remove(const Refptr<Texture>& texture);

		static std::string PathsToKey(const std::vector<std::string>& paths);
	private:
		static std::unordered_map<std::string, Refptr<Texture>> m_TextureNames;
		static std::unordered_map<std::string, Refptr<Texture>> m_TexturePaths;
	};

}
