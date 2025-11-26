#include "SNLpch.h"

#include "Snail/Render/Renderer/Renderer.h"

#include "Shader.h"

#include "Platform/glfw/OpenGL/Render/RenderAPI/OpenGLShader.h"

namespace Snail {

	std::shared_ptr<Shader> Shader::CreateShader(std::string vertexShaderSrc, std::string fragmentShaderSrc)
	{
		switch (Renderer::GetAPI()) {
		case RenderAPI::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
		case RenderAPI::OpenGL:		return std::make_shared<OpenGLShader>(vertexShaderSrc, fragmentShaderSrc);
		case RenderAPI::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!") return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

}