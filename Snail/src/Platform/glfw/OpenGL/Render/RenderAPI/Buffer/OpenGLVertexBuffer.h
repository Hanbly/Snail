#pragma once

#include "Snail/Core/Core.h"

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

		inline virtual std::shared_ptr<BufferLayout> OpenGLVertexBuffer::GetLayout() const override {
			return m_Layout;
		}
		inline virtual void OpenGLVertexBuffer::SetLayout(const std::shared_ptr<BufferLayout>& layout) override {
			SNL_CORE_ASSERT(layout->GetLayoutElements().data(), "OpenGLVertexBuffer: Layout数据为空, 保存布局失败!");
			m_Layout = layout;
		}

		virtual void Bind() const override;
		virtual void Unbind() const override;
	};

}