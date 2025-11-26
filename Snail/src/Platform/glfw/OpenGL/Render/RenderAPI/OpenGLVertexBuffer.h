#pragma once

#include "Snail/Core.h"

#include "Snail/Render/RenderAPI/VertexBuffer.h"

namespace Snail {

	class OpenGLVertexBuffer : public VertexBuffer {
	private:
		uint32_t m_BufferId;
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;
	};

}