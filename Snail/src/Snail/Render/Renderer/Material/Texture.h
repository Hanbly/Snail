#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

namespace Snail {

	enum class TextureType {
		None = 0,
		TWOD, Cube
	};

	static std::string TextureTypeToString(const TextureType& type)
	{
		switch (type) {
			case TextureType::TWOD: return "Texture2D";
			case TextureType::Cube: return "TextureCube";
			default: return "None";
		}
	}

	static std::string TextureUsageTypeToString(const std::string& type)
	{
		if (type.find("u_Diffuse") != std::string::npos) {
			return "texture_diffuse";
		}

		if (type.find("u_Specular") != std::string::npos) {
			return "texture_specular";
		}

		if (type.find("u_Cubemap") != std::string::npos) {
			return "texture_cubemap";
		}

		return type;
	}

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual const TextureType& GetType() const = 0;
		virtual const std::vector<std::string>& GetPath() const = 0;

		virtual void Bind(const uint32_t& slot = 0) const = 0;
		virtual void Unbind(const uint32_t& slot = 0) const = 0;
	};

	class Texture2D : public Texture {
	public:
		virtual ~Texture2D() = default;

		static Refptr<Texture2D> Create(const std::vector<std::string>& path);
	};

	class TextureCube : public Texture {
	public:
		virtual ~TextureCube() = default;

		static Refptr<TextureCube> Create(const std::vector<std::string>& path);
	};

}
