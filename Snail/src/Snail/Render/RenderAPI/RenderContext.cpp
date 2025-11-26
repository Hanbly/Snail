#include "SNLpch.h"

#include "Snail/Render/Renderer/Renderer.h"

#include "RenderContext.h"
#include "Platform/glfw/OpenGL/Render/RenderAPI/OpenGLRenderContext.h"

namespace Snail {

	std::unique_ptr<RenderContext> RenderContext::CreateRenderContext(void* windowHandle)
	{
		switch (Renderer::GetAPI()) {
			case RenderAPI::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
			case RenderAPI::OpenGL:		return std::make_unique<OpenGLRenderContext>(static_cast<GLFWwindow*>(windowHandle));
			case RenderAPI::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!"); return nullptr;
		}		
		
		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

}