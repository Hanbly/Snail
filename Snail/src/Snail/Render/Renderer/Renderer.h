#pragma once

#include "Snail/Core.h"

#include "Snail/Render/RenderAPI/VertexArray.h"
#include "Snail/Render/Renderer/RendererCommand.h"

namespace Snail {

	class SNAIL_API Renderer
	{
	public:
		Renderer() = default;
		~Renderer() = default;

		inline static RendererCommand::API GetAPI() {
			return RendererCommand::GetAPI();
		}

		static void BeginScene();
		static void EndScene();
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);
	};

}
