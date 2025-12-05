#pragma once

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

#include "Snail/Render/RenderAPI/VertexArray.h"

namespace Snail {

	class OpenGLVertexArray : public VertexArray {
	private:
		uint32_t m_ArrayId;
		uint32_t m_VertexBufferIndexOffset;
		std::vector<Refptr<VertexBuffer>> m_VertexBuffers;
		Refptr<IndexBuffer> m_IndexBuffer;
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		virtual void AddVertexBuffer(const Refptr<VertexBuffer>& vertexBuffer) override;
		virtual const Refptr<IndexBuffer> GetIndexBuffer() const override;
		virtual void SetIndexBuffer(const Refptr<IndexBuffer>& indexBuffer) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
	};

}