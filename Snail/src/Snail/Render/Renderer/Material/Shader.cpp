#include "SNLpch.h"

#include "Snail/Render/Renderer/RendererCommand.h"

#include "Shader.h"

#include "Platform/glfw/OpenGL/Render/Renderer/Material/OpenGLShader.h"

namespace Snail {

	Refptr<Shader> Shader::Create(const std::string& filePath)
	{
		switch (RendererCommand::GetAPI()) {
		case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
		case RendererCommand::API::OpenGL:		return std::make_shared<OpenGLShader>(filePath);
		case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!") return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

	Refptr<Shader> Shader::Create(const std::string& customName, const std::string& filePath)
	{
		switch (RendererCommand::GetAPI()) {
		case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
		case RendererCommand::API::OpenGL:		return std::make_shared<OpenGLShader>(customName, filePath);
		case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!") return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

	Refptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (RendererCommand::GetAPI()) {
		case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
		case RendererCommand::API::OpenGL:		return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!") return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

}