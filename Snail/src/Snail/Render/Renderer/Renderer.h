#pragma once

#include "Platform/glfw/OpenGL/Render/RenderAPI/OpenGLRenderContext.h"

namespace Snail {

	class SNAIL_API Renderer {
	private:
		RenderContext* m_RenderContext;
	public:
		Renderer() = default;
		~Renderer();

		void InitContext(void* windowHandle, RENDER_API_TYPE apiType);
		void SwapBuffers();
	};

}