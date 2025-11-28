#include "SNLpch.h"

#include "OpenGLRendererCommand.h"

namespace Snail {

	void OpenGLRendererCommand::ClearColorImpl(const glm::vec4& color_RGBA) const
	{
		glClearColor(color_RGBA.r, color_RGBA.g, color_RGBA.b, color_RGBA.a);
	}

	void OpenGLRendererCommand::ClearImpl() const
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void OpenGLRendererCommand::DrawIndexedImpl(const std::shared_ptr<VertexArray>& vertexArray) const
	{
		const uint32_t& count = vertexArray->GetIndexBuffer()->GetIndexBufferCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

}