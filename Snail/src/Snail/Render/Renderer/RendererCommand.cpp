#include "SNLpch.h"

#include "RendererCommand.h"
#include "Platform/glfw/OpenGL/Render/Renderer/OpenGLRendererCommand.h"

namespace Snail {

	RendererCommand::API RendererCommand::s_API = RendererCommand::API::OpenGL;

	std::unique_ptr<RendererCommand> RendererCommand::RC = RendererCommand::Create();

	std::unique_ptr<RendererCommand> RendererCommand::Create()
	{
		switch (RendererCommand::GetAPI()) {
		case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "Renderer: 取无效值 None!"); return nullptr;
		case RendererCommand::API::OpenGL:		return std::make_unique<OpenGLRendererCommand>();
		case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "Renderer: 暂不支持Vulkan!"); return nullptr;
		}

		SNL_CORE_ASSERT(false, "Renderer: switch无法取值!");
		return nullptr;
	}
}