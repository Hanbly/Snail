#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Buffer/VertexBuffer.h"
#include "Buffer/IndexBuffer.h"
#include "Buffer/BufferLayout.h"

namespace Snail {

	class SNAIL_API VertexArray {
	public:
		~VertexArray() {}

		virtual const std::vector<Vertex> GetVertexBuffer() const = 0;
		virtual void SetVertexBuffer(const Refptr<VertexBuffer>& vertexBuffer) = 0;

		virtual const Refptr<IndexBuffer>& GetIndexBuffer() const = 0;
		virtual void SetIndexBuffer(const Refptr<IndexBuffer>& indexBuffer) = 0;

		// 设置实例缓冲 (用于存储 Model 矩阵等每个实例独有的数据)
		virtual void SetInstanceBuffer(const Refptr<VertexBuffer>& vertexBuffer) = 0;
		virtual void UnbindInstanceBuffer() = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Refptr<VertexArray> Create();
	};

}