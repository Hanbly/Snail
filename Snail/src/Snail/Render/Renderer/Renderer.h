#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/RenderAPI/VertexArray.h"
#include "Snail/Render/Renderer/Material/Shader.h"
#include "Snail/Render/Renderer/RendererCommand.h"

#include "Camera/Camera.h"

namespace Snail {

	class SNAIL_API Renderer
	{
	public:
		Renderer() = delete;
		~Renderer() = default;

		inline static RendererCommand::API GetAPI() {
			return RendererCommand::GetAPI();
		}

		static void Init();
		static void SetViewPort(uint32_t width, uint32_t height);
	};

}
