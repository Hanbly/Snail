#include "SNLpch.h"

#include "Snail/Render/Renderer/RendererCommand.h"

#include "RenderContext.h"
#include "Platform/glfw/OpenGL/Render/RenderAPI/OpenGLRenderContext.h"

namespace Snail {

	std::unique_ptr<RenderContext> RenderContext::CreateRenderContext(void* windowHandle)
	{
		switch (RendererCommand::GetAPI()) {
			case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
			case RendererCommand::API::OpenGL:		return std::make_unique<OpenGLRenderContext>(static_cast<GLFWwindow*>(windowHandle));
			case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!"); return nullptr;
		}		
		
		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

}