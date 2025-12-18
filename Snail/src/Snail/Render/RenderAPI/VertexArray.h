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

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Refptr<VertexArray> Create();
	};

}