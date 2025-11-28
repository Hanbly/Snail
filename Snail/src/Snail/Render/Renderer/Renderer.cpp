#include "SNLpch.h"

#include "Renderer.h"

namespace Snail {

	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RendererCommand::RC->DrawIndexed(vertexArray);
	}

}