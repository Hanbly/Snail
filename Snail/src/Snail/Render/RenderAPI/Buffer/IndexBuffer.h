#pragma once

#include "Snail/Core.h"

namespace Snail {

	class SNAIL_API IndexBuffer {
	public:
		virtual ~IndexBuffer() {}

		virtual uint32_t GetIndexBufferCount() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static std::shared_ptr<IndexBuffer> CreateIndexBuffer(uint32_t* indices, uint32_t size);
	};

}