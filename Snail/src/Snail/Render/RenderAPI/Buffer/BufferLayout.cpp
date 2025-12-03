#include "SNLpch.h"

#include "Snail/Render/Renderer/RendererCommand.h"

#include "BufferLayout.h"
#include "Platform/glfw/OpenGL/Render/RenderAPI/Buffer/OpenGLBufferLayout.h"

namespace Snail {

	Refptr<BufferLayout> BufferLayout::Create(const std::initializer_list<VertexElement>& elements)
	{
		switch (RendererCommand::GetAPI()) {
			case RendererCommand::API::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
			case RendererCommand::API::OpenGL:		return std::make_shared<OpenGLBufferLayout>(elements);
			case RendererCommand::API::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!") return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

}