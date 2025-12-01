#pragma once

#include "Snail/Core/Core.h"

#include "Snail/Render/RenderAPI/RenderContext.h"

struct GLFWwindow;

namespace Snail {

	class OpenGLRenderContext :	public RenderContext
	{
	private:
		GLFWwindow* m_WindowHandle;
		static bool s_GLADInitialized;
	public:
		OpenGLRenderContext(GLFWwindow* window);
		~OpenGLRenderContext();

		virtual void InitContextImpl() override;
		virtual void SwapBuffersImpl() override;
	};

}


