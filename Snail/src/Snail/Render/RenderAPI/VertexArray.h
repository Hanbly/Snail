#pragma once

#include "Snail/Core/Core.h"

#include "Buffer/VertexBuffer.h"
#include "Buffer/IndexBuffer.h"
#include "Buffer/BufferLayout.h"

namespace Snail {

	class SNAIL_API VertexArray {
	public:
		~VertexArray() {}

		virtual void AddVertexBuffer(const Refptr<VertexBuffer>& vertexBuffer) = 0;
		virtual const Refptr<IndexBuffer> GetIndexBuffer() const = 0;
		virtual void SetIndexBuffer(const Refptr<IndexBuffer>& indexBuffer) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Refptr<VertexArray> Create();
	};

}