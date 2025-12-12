#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/RenderAPI/Buffer/BufferLayout.h"
#include "Snail/Render/RenderAPI/Buffer/VertexBuffer.h"

namespace Snail {

	class OpenGLVertexBuffer : public VertexBuffer {
	private:
		uint32_t m_BufferId;
		Refptr<BufferLayout> m_Layout;
	public:
		OpenGLVertexBuffer(const void* vertices, const uint32_t& size);
		~OpenGLVertexBuffer();

		inline virtual Refptr<BufferLayout> OpenGLVertexBuffer::GetLayout() const override {
			return m_Layout;
		}
		inline virtual void OpenGLVertexBuffer::SetLayout(const Refptr<BufferLayout>& layout) override {
			SNL_CORE_ASSERT(layout->GetLayoutElements().data(), "OpenGLVertexBuffer: Layout数据为空, 保存布局失败!");
			m_Layout = layout;
		}

		virtual void Bind() const override;
		virtual void Unbind() const override;
	};

}