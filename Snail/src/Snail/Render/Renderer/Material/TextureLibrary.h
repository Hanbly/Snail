#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Texture.h"

namespace Snail {

	class TextureLibrary {
	public:
		static Refptr<Texture> Load(const std::vector<std::string>& rawPath, const TextureUsage& usage);
		static Refptr<Texture> Load(const std::string& customName, const std::vector<std::string>& rawPath, const TextureUsage& usage);
		static Refptr<Texture> Load(const void* data, size_t size, const TextureUsage& usage, const std::string& cacheKey);

		static Refptr<Texture> Get(const std::string& name);

		static void Remove(const std::string& name);
		static void Remove(const Refptr<Texture>& texture);

		static Refptr<Texture> GetIBLIrradianceofTexture(const Refptr<Texture>& cubemap);
		static Refptr<Texture> GetIBLPrefilterofTexture(const Refptr<Texture>& cubemap);
	private:
		static std::vector<std::string> CleanFilePath(std::vector<std::string>& rawPath);
		// 同时生成IBL的漫反射辐照和镜面IBL
		static void GenIBLTextures(const Refptr<Texture>& cubemap);
	private:
		static std::unordered_map<std::string, Refptr<Texture>> m_TextureNames;
		static std::unordered_map<std::string, Refptr<Texture>> m_TexturePaths;
		static std::unordered_map<std::string, Refptr<Texture>> m_EmbeddedTexturePaths;
	};

}
