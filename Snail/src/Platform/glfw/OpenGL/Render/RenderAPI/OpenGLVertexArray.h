#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/RenderAPI/VertexArray.h"

namespace Snail {

	class OpenGLVertexArray : public VertexArray {
	private:
		uint32_t m_ArrayId;
		uint32_t m_VertexBufferIndexOffset;
		Refptr<VertexBuffer> m_VertexBuffer;
		Refptr<IndexBuffer> m_IndexBuffer;
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		virtual const std::vector<Vertex> GetVertexBuffer() const override;
		virtual void SetVertexBuffer(const Refptr<VertexBuffer>& vertexBuffer) override;

		virtual const Refptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }
		virtual void SetIndexBuffer(const Refptr<IndexBuffer>& indexBuffer) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
	};

}