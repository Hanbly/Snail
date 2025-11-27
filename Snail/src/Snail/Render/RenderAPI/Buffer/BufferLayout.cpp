#include "SNLpch.h"

#include "Snail/Render/Renderer/Renderer.h"

#include "BufferLayout.h"
#include "Platform/glfw/OpenGL/Render/RenderAPI/Buffer/OpenGLBufferLayout.h"

namespace Snail {

	std::shared_ptr<BufferLayout> BufferLayout::CreateBufferLayout(const std::initializer_list<VertexElement>& elements)
	{
		switch (Renderer::GetAPI()) {
		case RenderAPI::None:		SNL_CORE_ASSERT(false, "RenderAPI: 取无效值 None!"); return nullptr;
		case RenderAPI::OpenGL:		return std::make_shared<OpenGLBufferLayout>(elements);
		case RenderAPI::Vulkan:		SNL_CORE_ASSERT(false, "RenderAPI: 暂不支持Vulkan!") return nullptr;
		}

		SNL_CORE_ASSERT(false, "RenderAPI: switch无法取值!");
		return nullptr;
	}

}