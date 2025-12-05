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

		static void Init();
		static void SetViewPort(uint32_t width, uint32_t height);

		static void BeginScene(const Uniptr<Snail::Camera>& camera);
		static void EndScene();
		static void Submit(const Refptr<Shader>& shader, const Refptr<VertexArray>& vertexArray, const glm::mat4& model = glm::mat4(1.0f));
	};

}
