#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

namespace Snail {

	enum class TextureType {
		None = 0,
		TWOD, Cube
	};

	enum class TextureUsage {
		None = 0,
		UI,
		Diffuse, Specular, Cubemap, Normal,
		Albedo, Metallic, Roughness, AO, Emissive
	};

	static std::string TextureTypeToString(const TextureType& type)
	{
		switch (type) {
			case TextureType::TWOD: return "Texture2D";
			case TextureType::Cube: return "TextureCube";
			default: return "None";
		}
	}

	static std::string TextureUsageToString(const TextureUsage& type)
	{
		switch (type) {
			case TextureUsage::Diffuse: return "texture_diffuse";
			case TextureUsage::Specular: return "texture_specular";
			case TextureUsage::Cubemap: return "texture_cubemap";
			case TextureUsage::Normal: return "texture_normal";

			case TextureUsage::Albedo: return "texture_albedo";
			case TextureUsage::Metallic: return "texture_metallic";
			case TextureUsage::Roughness: return "texture_roughness";
			case TextureUsage::AO: return "texture_ao";
			default: return "None";
		}
	}

	static TextureUsage StringToTextureUsage(const std::string& typestr)
	{
		if (typestr == "texture_diffuse") return TextureUsage::Diffuse;
		if (typestr == "texture_specular") return TextureUsage::Specular;
		if (typestr == "texture_cubemap") return TextureUsage::Cubemap;
		if (typestr == "texture_normal") return TextureUsage::Normal;

		if (typestr == "texture_albedo") return TextureUsage::Albedo;
		if (typestr == "texture_metallic") return TextureUsage::Metallic;
		if (typestr == "texture_roughness") return TextureUsage::Roughness;
		if (typestr == "texture_ao") return TextureUsage::AO;
		return TextureUsage::None;
	}

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetRendererId() const = 0;
		virtual uint32_t GetUIRendererId() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual const TextureType& GetType() const = 0;
		virtual const TextureUsage& GetUsage() const = 0;
		virtual const std::vector<std::string>& GetPath() const = 0;

		virtual void SetPath(const std::vector<std::string>& path) = 0;

		virtual bool& GetEnable() = 0;
		virtual void SetEnable(bool enable) = 0;

		virtual void Bind(const uint32_t& slot = 0) const = 0;
		virtual void Unbind(const uint32_t& slot = 0) const = 0;
	};

	class Texture2D : public Texture {
	public:
		friend class TextureLibrary;
		friend class RendererTools;
		virtual ~Texture2D() = default;

		static void BindExternal(const uint32_t& slot, const uint32_t& rendererId);
	private:
		static Refptr<Texture2D> Create(const std::vector<std::string>& path, const TextureUsage& usage);
		static Refptr<Texture2D> Create(const void* data, size_t size, const TextureUsage& usage);

		virtual void LoadTexture2D(const std::vector<std::string>& path) = 0;

	};

	class TextureCube : public Texture {
	public:
		friend class TextureLibrary;
		friend class RendererTools;
		virtual ~TextureCube() = default;
	private:
		static Refptr<TextureCube> Create(const int dim, const bool mipmap = false);
		static Refptr<TextureCube> Create(const std::vector<std::string>& path, const TextureUsage& usage);

		virtual void LoadCubemapFromFaces(const std::vector<std::string>& path) = 0;
	};

}
