#pragma once

#include "Snail/Core/Core.h"

#include "Snail/Render/RenderAPI/VertexArray.h"
#include "Snail/Render/RenderAPI/Shader.h"
#include "Snail/Render/Renderer/RendererCommand.h"

#include "Camera/Camera.h"

namespace Snail {

	class SNAIL_API Renderer
	{
	private:
		struct RendererSceneData {
			glm::mat4 viewMatrix;
			glm::mat4 projectionMatrix;
		};
		static RendererSceneData m_SceneData;
	public:
		Renderer() = default;
		~Renderer() = default;

		inline static RendererCommand::API GetAPI() {
			return RendererCommand::GetAPI();
		}

		static void BeginScene(const std::unique_ptr<Snail::Camera>& camera);
		static void EndScene();
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);
	};

}
