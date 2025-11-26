#pragma once

#include "Snail/Core.h"

namespace Snail {

	enum class SNAIL_API RenderAPI {
		None = 0,
		OpenGL,
		Vulkan
	};

	class SNAIL_API Renderer
	{
	private:
		static RenderAPI s_RenderAPI;
	public:
		Renderer() = default;
		~Renderer() = default;

		inline static RenderAPI GetAPI() {
			return s_RenderAPI;
		}

		inline static RenderAPI SetAPI(RenderAPI api) {
			s_RenderAPI = api;
		}
	};

}
