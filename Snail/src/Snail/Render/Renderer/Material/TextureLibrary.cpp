#include "SNLpch.h"

#include "ShaderLibrary.h"
#include "TextureLibrary.h"

#include "Snail/Render/RenderAPI/VertexArray.h"
#include "Snail/Render/Renderer/FrameBuffer/FrameBuffer.h"
#include "Snail/Render/Renderer/RendererCommand.h"
#include "Snail/Utils/RendererTools.h"

namespace Snail {

	std::unordered_map<std::string, Refptr<Texture>> TextureLibrary::m_TextureNames = std::unordered_map<std::string, Refptr<Texture>>();
	std::unordered_map<std::string, Refptr<Texture>> TextureLibrary::m_TexturePaths = std::unordered_map<std::string, Refptr<Texture>>();
	std::unordered_map<std::string, Refptr<Texture>> TextureLibrary::m_EmbeddedTexturePaths = std::unordered_map<std::string, Refptr<Texture>>();
	int TextureLibrary::m_PrefilterMapResolution = 256;
	int TextureLibrary::m_PrefilterMipLevels = 5;

	void TextureLibrary::Clear()
	{
		m_TextureNames.clear();
		m_TexturePaths.clear();
		m_EmbeddedTexturePaths.clear();
	}

	Refptr<Texture> TextureLibrary::Load(const std::vector<std::string>& rawPath, const TextureUsage& usage)
	{
		std::vector<std::string> filePath = rawPath;
		filePath = CleanFilePath(filePath);
		// 从路径提取名字 (例如 assets/images/color.png -> color)
		std::filesystem::path path = filePath[0];		// 默认首个
		std::string name = path.u8string() + "_" + TextureUsageToString(usage);

		std::string pathKey = RendererTools::TexturePathsToKey(filePath, usage);
		
		if (m_TexturePaths.find(pathKey) != m_TexturePaths.end()) { // 资源已经加载过
			if (m_TextureNames.find(name) == m_TextureNames.end()) { // 但是找不到对应命名
				m_TextureNames[name] = m_TexturePaths[pathKey]; // 把 pathKey 对应的纹理对象添加到命名映射表
			} // else 否则就是完全相同，不做操作
			return m_TexturePaths[pathKey];
		}

		if (m_TextureNames.find(name) != m_TextureNames.end()) { // 命名已经存在
			SNL_CORE_WARN("TextureLibrary: 名字 '{0}' 已被占用，但指向的是不同资源！", name); // 新加载一个纹理
			//return m_TextureNames[customName];
		}

		Refptr<Texture> texture;
		if (filePath.size() == 1 && usage != TextureUsage::Cubemap) {
			texture = Texture2D::Create(filePath, usage);	
		}
		else if (filePath.size() == 1 && usage == TextureUsage::Cubemap) {
			texture = RendererTools::LoadEquirectangularToCubemap(filePath[0]);
			GenIBLTextures(texture);
		}
		else if (filePath.size() > 1 && usage == TextureUsage::Cubemap) {
			texture = TextureCube::Create(filePath, usage);
			GenIBLTextures(texture);
		}
		m_TextureNames[name] = texture;
		m_TexturePaths[pathKey] = texture;

		return texture;
	}

	Refptr<Texture> TextureLibrary::Load(const std::string& customName, const std::vector<std::string>& rawPath, const TextureUsage& usage)
	{
		std::vector<std::string> filePath = rawPath;
		filePath = CleanFilePath(filePath);
		std::string pathKey = RendererTools::TexturePathsToKey(filePath, usage);

		if (m_TexturePaths.find(pathKey) != m_TexturePaths.end()) { // 资源已经加载过
			if (m_TextureNames.find(customName) == m_TextureNames.end()) { // 但是找不到对应命名
				m_TextureNames[customName] = m_TexturePaths[pathKey]; // 把 pathKey 对应的纹理对象添加到命名映射表
			} // else 否则就是完全相同，不做操作
			return m_TexturePaths[pathKey];
		}

		if (m_TextureNames.find(customName) != m_TextureNames.end()) { // 命名已经存在
			SNL_CORE_WARN("TextureLibrary: 名字 '{0}' 已被占用，但指向的是不同资源！", customName); // 新加载一个纹理
			//return m_TextureNames[customName];
		}

		Refptr<Texture> texture;
		if (filePath.size() == 1 && usage != TextureUsage::Cubemap) {
			texture = Texture2D::Create(filePath, usage);
		}
		else if (filePath.size() == 1 && usage == TextureUsage::Cubemap) {
			texture = RendererTools::LoadEquirectangularToCubemap(filePath[0]);
			GenIBLTextures(texture);
		}
		else if (filePath.size() > 1 && usage == TextureUsage::Cubemap) {
			texture = TextureCube::Create(filePath, usage);
			GenIBLTextures(texture);
		}
		m_TextureNames[customName] = texture;
		m_TexturePaths[pathKey] = texture;

		return texture;
	}

	Refptr<Texture> TextureLibrary::Load(const void* data, size_t size, const TextureUsage& usage, const std::string& pathKey)
	{
		if (m_EmbeddedTexturePaths.find(pathKey) != m_EmbeddedTexturePaths.end()) { // 资源已经加载过
			return m_EmbeddedTexturePaths[pathKey];
		}

		if (Refptr<Texture> texture = Texture2D::Create(data, size, usage)) {
			m_EmbeddedTexturePaths[pathKey] = texture;
			return texture;
		}
		return nullptr;
	}

	Refptr<Texture> TextureLibrary::Get(const std::string& name)
	{
		int status = m_TextureNames.find(name) != m_TextureNames.end(); // 1表示name在映射表中已经存在
		SNL_CORE_ASSERT(status, "TextureLibrary: 纹理库不存在该 texture '{0}' ", name);

		return m_TextureNames[name];
	}

	void TextureLibrary::Remove(const std::string& name)
	{
		if (m_TextureNames.find(name) != m_TextureNames.end()) {
			m_TextureNames.erase(name);
		}
	}

	void TextureLibrary::Remove(const Refptr<Texture>& texture)
	{
		std::string path = RendererTools::TexturePathsToKey(texture->GetPath(), texture->GetUsage());
		if (m_TexturePaths.find(path) != m_TexturePaths.end()) {
			m_TexturePaths.erase(path);
		}
	}

	Refptr<Texture> TextureLibrary::GetIBLIrradianceofTexture(const Refptr<Texture>& cubemap)
	{
		auto& filePath = cubemap->GetPath();
		auto usage = cubemap->GetUsage();
		if (filePath.size() == 1 && usage != TextureUsage::Cubemap) {
			SNL_CORE_WARN("TextureLibrary::GetIBLIrradianceofTexture 警告: 试图获取 2D纹理 IBL!");
			return nullptr;
		}
		// 从路径提取名字
		std::filesystem::path path = filePath[0];
		std::string irradianceName = "IBL_Irradiance_CubemapTexture_" + path.u8string() + "_" + TextureUsageToString(usage);

		return Get(irradianceName);
	}

	Refptr<Texture> TextureLibrary::GetIBLPrefilterofTexture(const Refptr<Texture>& cubemap)
	{
		auto& filePath = cubemap->GetPath();
		auto usage = cubemap->GetUsage();
		if (filePath.size() == 1 && usage != TextureUsage::Cubemap) {
			SNL_CORE_WARN("TextureLibrary::GetIBLPrefilterofTexture 警告: 试图获取 2D纹理 IBL!");
			return nullptr;
		}
		// 从路径提取名字
		std::filesystem::path path = filePath[0];
		std::string prefilterName = "IBL_Prefilter_CubemapTexture_" + path.u8string() + "_" + TextureUsageToString(usage);

		return Get(prefilterName);
	}

	std::vector<std::string> TextureLibrary::CleanFilePath(std::vector<std::string>& rawPath)
	{
		for (auto& path : rawPath) {
			path = RendererTools::CleanFilePath(path);
			path = RendererTools::CleanWindowsPath(path);
		}
		return rawPath;
	}

	void TextureLibrary::GenIBLTextures(const Refptr<Texture>& cubemap)
	{
		auto& filePath = cubemap->GetPath();
		auto usage = cubemap->GetUsage();
		if (filePath.size() == 1 && usage != TextureUsage::Cubemap) {
			SNL_CORE_WARN("TextureLibrary::GenIBLTextures 警告: 试图为 2D纹理 扩展 IBL!");
			return;
		}

		// 从路径提取名字
		std::filesystem::path path = filePath[0];		// 默认首个
		std::string irradianceName = "IBL_Irradiance_CubemapTexture_" + path.u8string() + "_" + TextureUsageToString(usage);
		std::string prefilterName = "IBL_Prefilter_CubemapTexture_" + path.u8string() + "_" + TextureUsageToString(usage);

		std::string irradiancePathKey = "IBL_Irradiance_CubemapTexture_" + RendererTools::TexturePathsToKey(filePath, usage);
		std::string prefilterPathKey = "IBL_Prefilter_CubemapTexture_" + RendererTools::TexturePathsToKey(filePath, usage);

		if (m_TexturePaths.find(irradiancePathKey) != m_TexturePaths.end()) { // 资源已经加载过
			if (m_TextureNames.find(irradianceName) == m_TextureNames.end()) { // 但是找不到对应命名
				m_TextureNames[irradianceName] = m_TexturePaths[irradiancePathKey]; // 把 pathKey 对应的纹理对象添加到命名映射表
			} // else 否则就是完全相同，不做操作
		}

		if (m_TextureNames.find(irradianceName) != m_TextureNames.end()) { // 命名已经存在
			SNL_CORE_WARN("TextureLibrary::GenIBLTextures 名字 '{0}' 已被占用，但指向的是不同资源！", irradianceName); // 新加载一个纹理
		}

		if (m_TexturePaths.find(prefilterPathKey) != m_TexturePaths.end()) { // 资源已经加载过
			if (m_TextureNames.find(prefilterName) == m_TextureNames.end()) { // 但是找不到对应命名
				m_TextureNames[prefilterName] = m_TexturePaths[prefilterPathKey]; // 把 pathKey 对应的纹理对象添加到命名映射表
			} // else 否则就是完全相同，不做操作
		}

		if (m_TextureNames.find(prefilterName) != m_TextureNames.end()) { // 命名已经存在
			SNL_CORE_WARN("TextureLibrary::GenIBLTextures 名字 '{0}' 已被占用，但指向的是不同资源！", prefilterName); // 新加载一个纹理
		}

		Refptr<Texture> irradianceTexture;
		Refptr<Texture> prefilterTexture;
		if (usage == TextureUsage::Cubemap) {
			irradianceTexture = RendererTools::CalculateCubemapToIBLIrradianceConvolution(cubemap);
			prefilterTexture = RendererTools::CalculateCubemapToIBLPrefilter(cubemap, m_PrefilterMapResolution, m_PrefilterMipLevels);
		}
		m_TextureNames[irradianceName] = irradianceTexture;
		m_TexturePaths[irradiancePathKey] = irradianceTexture;
		m_TextureNames[prefilterName] = prefilterTexture;
		m_TexturePaths[prefilterPathKey] = prefilterTexture;
	}

}