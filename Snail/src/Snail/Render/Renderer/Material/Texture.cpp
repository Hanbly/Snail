#include "SNLpch.h"

#include "Snail/Render/Renderer/RendererCommand.h"

#include "Texture.h"
#include "Platform/glfw/OpenGL/Render/Renderer/Material/OpenGLTexture.h"

namespace Snail {

	Refptr<Texture2D> Texture2D::Create(const std::string& path) {
		switch (RendererCommand::GetAPI()) {
			case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
			case RendererCommand::API::OpenGL:		return std::make_shared<OpenGLTexture2D>(path);
			case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!"); return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

	Refptr<TextureCube> TextureCube::Create(const std::array<std::string, 6>& paths)
	{
		switch (RendererCommand::GetAPI()) {
		case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
		case RendererCommand::API::OpenGL:		return std::make_shared<OpenGLTextureCube>(paths);
		case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!"); return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

}