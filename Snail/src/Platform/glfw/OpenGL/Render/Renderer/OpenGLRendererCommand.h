#pragma once

#include "Snail/Core.h"

#include "Snail/Render/Renderer/RendererCommand.h"

namespace Snail {

	class OpenGLRendererCommand : public RendererCommand
	{
	public:
		OpenGLRendererCommand() = default;
		~OpenGLRendererCommand() = default;

		virtual void EnableDepthTestImpl() const override;
		virtual void ClearColorImpl(const glm::vec4& color_RGBA) const override;
		virtual void ClearImpl() const override;
		virtual void DrawIndexedImpl(const std::shared_ptr<VertexArray>& vertexArray) const override;
	};

}
