#include "SNLpch.h"

#include "Snail/Render/Renderer/RendererCommand.h"

#include "VertexBuffer.h"
#include "Platform/glfw/OpenGL/Render/RenderAPI/Buffer/OpenGLVertexBuffer.h"

namespace Snail {

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (RendererCommand::GetAPI()) {
			case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
			case RendererCommand::API::OpenGL:		return std::make_shared<OpenGLVertexBuffer>(vertices, size);
			case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!") return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

}