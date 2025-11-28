#pragma once

#include "Snail/Core.h"

#include "Snail/Render/RenderAPI/VertexArray.h"

namespace Snail {

	class OpenGLVertexArray : public VertexArray {
	private:
		uint32_t m_ArrayId;
		uint32_t m_VertexBufferIndexOffset;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual const std::shared_ptr<IndexBuffer> GetIndexBuffer() const override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
	};

}