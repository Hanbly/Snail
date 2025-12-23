#include "SNLpch.h"

#include "Snail/Render/Renderer/RendererCommand.h"

#include "Texture.h"
#include "Platform/glfw/OpenGL/Render/Renderer/Material/OpenGLTexture.h"

namespace Snail {

	Refptr<Texture2D> Texture2D::Create(const std::vector<std::string>& path, const TextureUsage& usage) {
		switch (RendererCommand::GetAPI()) {
			case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
			case RendererCommand::API::OpenGL:		return std::make_shared<OpenGLTexture2D>(path, usage);
			case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!"); return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

	Refptr<TextureCube> TextureCube::Create(const std::vector<std::string>& path, const TextureUsage& usage)
	{
		switch (RendererCommand::GetAPI()) {
			case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
			case RendererCommand::API::OpenGL:		return std::make_shared<OpenGLTextureCube>(path, usage);
			case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!"); return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}


	void Texture2D::BindExternal(const uint32_t& slot, const uint32_t& rendererId)
	{
		switch (RendererCommand::GetAPI()) {
			case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); break;
			case RendererCommand::API::OpenGL:		OpenGLTexture2D::BindExternal(slot, rendererId); break;
			case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!"); break;
		}
	}

}