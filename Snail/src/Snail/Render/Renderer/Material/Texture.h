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
		Diffuse, Specular, Cubemap
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
			default: return "None";
		}
	}

	static TextureUsage StringToTextureUsage(const std::string& typestr)
	{
		if (typestr == "texture_diffuse") return TextureUsage::Diffuse;
		if (typestr == "texture_specular") return TextureUsage::Specular;
		if (typestr == "texture_cubemap") return TextureUsage::Cubemap;

		return TextureUsage::None;
	}

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetRendererId() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual const TextureType& GetType() const = 0;
		virtual const TextureUsage& GetUsage() const = 0;
		virtual const std::vector<std::string>& GetPath() const = 0;

		virtual void Bind(const uint32_t& slot = 0) const = 0;
		virtual void Unbind(const uint32_t& slot = 0) const = 0;
	};

	class Texture2D : public Texture {
	public:
		friend class TextureLibrary;
		virtual ~Texture2D() = default;

		static void BindExternal(const uint32_t& slot, const uint32_t& rendererId);
	private:
		static Refptr<Texture2D> Create(const std::vector<std::string>& path, const TextureUsage& usage);
	};

	class TextureCube : public Texture {
	public:
		friend class TextureLibrary;
		virtual ~TextureCube() = default;
	private:
		static Refptr<TextureCube> Create(const std::vector<std::string>& path, const TextureUsage& usage);
	};

}
