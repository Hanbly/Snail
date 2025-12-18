#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/Renderer/Material/Texture.h"

namespace Snail {

	class OpenGLTexture2D : public Texture2D {
	private:
		uint32_t m_RendererId;
		uint32_t m_Width;
		uint32_t m_Height;

		TextureType m_Type;
		std::vector<std::string> m_Path;
	public:
		OpenGLTexture2D(const std::vector<std::string>& path);
		virtual ~OpenGLTexture2D();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }
		inline virtual const TextureType& GetType() const override { return m_Type; }
		inline virtual const std::vector<std::string>& GetPath() const override { return m_Path; }

		virtual void Bind(const uint32_t& slot = 0) const override;
		virtual void Unbind(const uint32_t& slot = 0) const override;
	};

	class OpenGLTextureCube : public TextureCube {
	private:
		uint32_t m_RendererId;
		uint32_t m_Width;
		uint32_t m_Height;

		TextureType m_Type;
		std::vector<std::string> m_Path;
	public:
		OpenGLTextureCube(const std::vector<std::string>& path);
		virtual ~OpenGLTextureCube();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }
		inline virtual const TextureType& GetType() const override { return m_Type; }
		inline virtual const std::vector<std::string>& GetPath() const override { return m_Path; }

		virtual void Bind(const uint32_t& slot = 0) const override;
		virtual void Unbind(const uint32_t& slot = 0) const override;
	};

}