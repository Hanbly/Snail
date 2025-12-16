#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

namespace Snail {

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual std::string GetPath() const = 0;

		virtual void Bind(const uint32_t& slot = 0) const = 0;
		virtual void Unbind(const uint32_t& slot = 0) const = 0;
	};

	class Texture2D : public Texture {
	public:
		virtual ~Texture2D() = default;

		static Refptr<Texture2D> Create(const std::string& path);
	};

	class TextureCube : public Texture {
	public:
		virtual ~TextureCube() = default;

		static Refptr<TextureCube> Create(const std::array<std::string, 6>& paths);
	};

}
