#include "SNLpch.h"

#include "Snail/Render/Renderer/RendererCommand.h"

#include "Shader.h"

#include "Platform/glfw/OpenGL/Render/RenderAPI/OpenGLShader.h"

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

	// ------------------------ Shader Library ------------------------------------------##################################

	void ShaderLibrary::Add(const Refptr<Shader>& shader)
	{
		SNL_CORE_ASSERT(shader->GetName().size(), "ShaderLibrary: 着色器命名缺失，shaderId '{0}' ", shader->GetRendererId());

		int status = m_Shaders.find(shader->GetName()) == m_Shaders.end(); // 1表示shader在映射表中不存在
		SNL_CORE_ASSERT(status, "ShaderLibrary: 着色器库已经存在该 shader '{0}' ", shader->GetName());

		m_Shaders[shader->GetName()] = shader;
	}

	Refptr<Shader> ShaderLibrary::Load(const std::string& filePath)
	{
		const Refptr<Shader>& shader = Shader::Create(filePath);
		Add(shader);
		return shader;
	}

	Refptr<Shader> ShaderLibrary::Load(const std::string& customName, const std::string& filePath)
	{
		const Refptr<Shader>& shader = Shader::Create(customName, filePath);
		Add(shader);
		return shader;
	}

	Refptr<Shader> ShaderLibrary::Get(const std::string& name)
	{
		int status = m_Shaders.find(name) != m_Shaders.end(); // 1表示name在映射表中已经存在
		SNL_CORE_ASSERT(status, "ShaderLibrary: 着色器库不存在该 shader name '{0}' ", name);

		return m_Shaders[name];
	}

}