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
		bool m_SettedInstanceBuffer = false;
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		virtual const std::vector<Vertex> GetVertexBuffer() const override;
		virtual void SetVertexBuffer(const Refptr<VertexBuffer>& vertexBuffer) override;

		virtual const Refptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }
		virtual void SetIndexBuffer(const Refptr<IndexBuffer>& indexBuffer) override;

		// 设置某个 实例化属性数组 到 this（vao）的布局属性之中，之后在shader通过顶点属性（location = n）获取
		virtual void SetInstanceBuffer(const Refptr<VertexBuffer>& vertexBuffer) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
	};

}