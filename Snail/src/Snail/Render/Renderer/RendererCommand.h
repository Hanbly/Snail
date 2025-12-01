#pragma once

#include "Snail/Core/Core.h"

#include "Snail/Render/RenderAPI/VertexArray.h"

#include "glm/glm.hpp"

namespace Snail {

	class SNAIL_API RendererCommand
	{
	public:
		enum class API {
			None = 0,
			OpenGL,
			Vulkan
		};
		virtual ~RendererCommand() = default;

		inline static API GetAPI() {
			return s_API;
		}
	private:
		static API s_API;
	private: //-------------------------Commands-------------------------------------
		static std::unique_ptr<RendererCommand> RC;

		virtual void EnableDepthTestImpl() const = 0;
		virtual void ClearColorImpl(const glm::vec4& color_RGBA) const = 0;
		virtual void ClearImpl() const = 0;
		virtual void DrawIndexedImpl(const std::shared_ptr<VertexArray>& vertexArray) const = 0;
	public:
		inline static void EnableDepthTest() {
			RC->EnableDepthTestImpl();
		}
		inline static void ClearColor(const glm::vec4& color_RGBA) {
			RC->ClearColorImpl(color_RGBA);
		}
		inline static void Clear() {
			RC->ClearImpl();
		}
		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
			RC->DrawIndexedImpl(vertexArray);
		}

		static std::unique_ptr<RendererCommand> CreateRendererCommand();

		friend class Renderer;
	};

}
