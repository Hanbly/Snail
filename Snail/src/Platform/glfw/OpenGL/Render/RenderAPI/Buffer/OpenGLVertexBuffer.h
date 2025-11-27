#pragma once

#include "Snail/Core.h"

#include "Snail/Render/RenderAPI/Buffer/BufferLayout.h"
#include "Snail/Render/RenderAPI/Buffer/VertexBuffer.h"

namespace Snail {

	class OpenGLVertexBuffer : public VertexBuffer {
	private:
		uint32_t m_BufferId;
		std::shared_ptr<BufferLayout> m_Layout;
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		~OpenGLVertexBuffer();

		virtual void SetLayout(const std::shared_ptr<BufferLayout>& layout) override;
		virtual void EnableLayout(bool enableNormalize) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
	};

}