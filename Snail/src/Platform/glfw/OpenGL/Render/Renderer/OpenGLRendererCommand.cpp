#include "SNLpch.h"

#include "OpenGLRendererCommand.h"

namespace Snail {

	void OpenGLRendererCommand::InitImpl()
	{
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRendererCommand::SetViewPortImpl(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererCommand::ClearColorImpl(const glm::vec4& color_RGBA) const
	{
		glClearColor(color_RGBA.r, color_RGBA.g, color_RGBA.b, color_RGBA.a);
	}

	void OpenGLRendererCommand::ClearImpl() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererCommand::DrawIndexedImpl(const Refptr<VertexArray>& vertexArray) const
	{
		const uint32_t& count = vertexArray->GetIndexBuffer()->GetIndexBufferCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

}