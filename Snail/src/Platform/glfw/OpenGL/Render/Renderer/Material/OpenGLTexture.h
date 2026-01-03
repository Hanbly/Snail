#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/Renderer/Material/Texture.h"

namespace Snail {

	class OpenGLTexture2D : public Texture2D {
	private:
		uint32_t m_RendererId;
		uint32_t m_UIId = 0; // UI 视图 ID
		uint32_t m_Width;
		uint32_t m_Height;

		TextureType m_Type;
		TextureUsage m_Usage;
		std::vector<std::string> m_Path;
	public:
		OpenGLTexture2D(const std::vector<std::string>& path, const TextureUsage& usage);
		virtual ~OpenGLTexture2D();

		inline virtual uint32_t GetRendererId() const override { return m_RendererId; }
		// 根据是否线性转换（sRGB），选择返回UIid还是RendererId
		inline virtual uint32_t GetUIRendererId() const override { return m_UIId > 0 ? m_UIId : m_RendererId; }
		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }
		inline virtual const TextureType& GetType() const override { return m_Type; }
		inline virtual const TextureUsage& GetUsage() const override { return m_Usage; }
		inline virtual const std::vector<std::string>& GetPath() const override { return m_Path; }

		inline virtual void SetPath(const std::vector<std::string>& path) override { m_Path = path; }

		virtual void Bind(const uint32_t& slot = 0) const override;
		virtual void Unbind(const uint32_t& slot = 0) const override;

		static void BindExternal(const uint32_t& slot, const uint32_t& rendererId);
	private:
		virtual void LoadTexture2D(const std::vector<std::string>& path) override;
	};

	class OpenGLTextureCube : public TextureCube {
	private:
		uint32_t m_RendererId;
		uint32_t m_UIId = 0; // UI 视图 ID
		uint32_t m_Width;
		uint32_t m_Height;

		TextureType m_Type;
		TextureUsage m_Usage;
		std::vector<std::string> m_Path;
	public:
		OpenGLTextureCube(const int dim);
		OpenGLTextureCube(const std::vector<std::string>& path, const TextureUsage& usage);
		virtual ~OpenGLTextureCube();

		inline virtual uint32_t GetRendererId() const override { return m_RendererId; }
		// 根据是否线性转换（sRGB），选择返回UIid还是RendererId
		inline virtual uint32_t GetUIRendererId() const override { return m_UIId > 0 ? m_UIId : m_RendererId; }
		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }
		inline virtual const TextureType& GetType() const override { return m_Type; }
		inline virtual const TextureUsage& GetUsage() const override { return m_Usage; }
		inline virtual const std::vector<std::string>& GetPath() const override { return m_Path; }

		inline virtual void SetPath(const std::vector<std::string>& path) override { m_Path = path; }

		virtual void Bind(const uint32_t& slot = 0) const override;
		virtual void Unbind(const uint32_t& slot = 0) const override;

	private:
		virtual void LoadCubemapFromFaces(const std::vector<std::string>& path) override;
	};

}