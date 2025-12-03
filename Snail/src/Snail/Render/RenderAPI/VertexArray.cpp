#include "SNLpch.h"

#include "Snail/Render/Renderer/RendererCommand.h"

#include "VertexArray.h"
#include "Platform/glfw/OpenGL/Render/RenderAPI/OpenGLVertexArray.h"

namespace Snail {

	Refptr<VertexArray> VertexArray::Create() {
		switch (RendererCommand::GetAPI()) {
		case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
		case RendererCommand::API::OpenGL:		return std::make_shared<OpenGLVertexArray>();
		case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!") return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

}