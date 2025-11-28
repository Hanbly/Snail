#pragma once

#include "Snail/Core.h"

#include "Buffer/VertexBuffer.h"
#include "Buffer/IndexBuffer.h"
#include "Buffer/BufferLayout.h"

namespace Snail {

	class SNAIL_API VertexArray {
	public:
		~VertexArray() {}

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
		virtual const std::shared_ptr<IndexBuffer> GetIndexBuffer() const = 0;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static std::shared_ptr<VertexArray> CreateVertexArray();
	};

}