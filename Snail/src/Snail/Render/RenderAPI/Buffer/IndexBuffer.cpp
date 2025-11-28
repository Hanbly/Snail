#include "SNLpch.h"

#include "Snail/Render/Renderer/RendererCommand.h"

#include "IndexBuffer.h"
#include "Platform/glfw/OpenGL/Render/RenderAPI/Buffer/OpenGLIndexBuffer.h"

namespace Snail {

	std::shared_ptr<IndexBuffer> IndexBuffer::CreateIndexBuffer(uint32_t* indices, uint32_t size)
	{
		switch (RendererCommand::GetAPI()) {
			case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
			case RendererCommand::API::OpenGL:		return std::make_shared<OpenGLIndexBuffer>(indices, size);
			case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!") return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

}