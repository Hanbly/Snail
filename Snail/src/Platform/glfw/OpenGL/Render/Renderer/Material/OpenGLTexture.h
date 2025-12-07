#pragma once

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

#include "Snail/Render/Renderer/Material/Texture.h"

namespace Snail {

	class OpenGLTexture2D : public Texture2D
	{
	private:
		uint32_t m_RendererId;
		uint32_t m_Width;
		uint32_t m_Height;
		std::string m_Path;
	public:
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }
		inline virtual std::string GetPath() const override { return m_Path; }

		virtual void Bind(const uint32_t& slot = 0) const override;
		virtual void Unbind(const uint32_t& slot = 0) const override;
	};

}