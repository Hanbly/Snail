#include "SNLpch.h"

#include "Renderer.h"

namespace Snail {

	Renderer::RendererSceneData Renderer::m_SceneData = Renderer::RendererSceneData();

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

		shader->SetUniformMatrix4fv("model", model);
		shader->SetUniformMatrix4fv("view", Renderer::m_SceneData.viewMatrix);
		shader->SetUniformMatrix4fv("projection", Renderer::m_SceneData.projectionMatrix);

		RendererCommand::RC->DrawIndexed(vertexArray);
	}

}