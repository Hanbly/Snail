#include "SNLpch.h"

#include "Renderer.h"

namespace Snail {
	
	Renderer::~Renderer()
	{
		delete m_RenderContext;
	}

	// 初始化图形API的渲染上下文
	void Renderer::InitContext(void* windowHandle, RENDER_API_TYPE apiType) {
		switch (apiType) {
			case RENDER_API_TYPE::GLFW_OPENGL: {
				GLFWwindow* windowHandle_GLFW = static_cast<GLFWwindow*>(windowHandle);
				m_RenderContext = new OpenGLRenderContext(windowHandle_GLFW);
				break;
			}
			case RENDER_API_TYPE::GLFW_VULKAN: {
				break;
			}
			default:
				SNL_CORE_ASSERT(0, "渲染器错误断言: 不合法 RENDER_API_TYPE");
		}
		m_RenderContext->InitContext();
	}
	void Renderer::SwapBuffers() {
		SNL_CORE_ASSERT(m_RenderContext, "渲染器错误断言: 未初始化上下文!");
		m_RenderContext->SwapBuffers();
	}

}