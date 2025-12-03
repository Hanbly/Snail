#include "SNLpch.h"

#include "Renderer.h"

namespace Snail {

	Renderer::RendererSceneData Renderer::m_SceneData = Renderer::RendererSceneData();

	void Renderer::Init()
	{
		RendererCommand::Init();
	}

	void Renderer::BeginScene(const Uniptr<Snail::Camera>& camera)
	{
		m_SceneData.viewMatrix = camera->GetViewMatrix();
		m_SceneData.projectionMatrix = camera->GetProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Refptr<Shader>& shader, const Refptr<VertexArray>& vertexArray, const glm::mat4& model)
	{
		vertexArray->Bind();
		shader->Bind();

		shader->SetMat4("model", model);
		shader->SetMat4("view", Renderer::m_SceneData.viewMatrix);
		shader->SetMat4("projection", Renderer::m_SceneData.projectionMatrix);

		RendererCommand::DrawIndexed(vertexArray);
	}

}