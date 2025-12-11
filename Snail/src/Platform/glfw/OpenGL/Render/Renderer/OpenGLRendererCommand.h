#pragma once

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

#include "Snail/Render/Renderer/RendererCommand.h"

namespace Snail {

	class OpenGLRendererCommand : public RendererCommand
	{
	public:
		OpenGLRendererCommand() = default;
		~OpenGLRendererCommand() = default;

		virtual void InitImpl() override;
		virtual void SetViewPortImpl(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void ClearColorImpl(const glm::vec4& color_RGBA) const override;
		virtual void ClearImpl() const override;
		virtual void DrawIndexedImpl(const Refptr<VertexArray>& vertexArray) const override;

		virtual void StencilFuncImpl(const StencilFuncType& type, const int& ref, const int& mask) const override;
		virtual void StencilMaskImpl(const bool& status) const override;
		virtual void DepthTestImpl(const bool& enable) const override;
	};

}
