#pragma once

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

namespace Snail {

	class SNAIL_API IndexBuffer {
	public:
		virtual ~IndexBuffer() {}

		virtual uint32_t GetIndexBufferCount() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Refptr<IndexBuffer> Create(uint32_t* indices, uint32_t size);
	};

}