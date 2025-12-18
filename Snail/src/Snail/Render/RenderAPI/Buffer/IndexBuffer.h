#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

namespace Snail {

	class SNAIL_API IndexBuffer {
	public:
		virtual ~IndexBuffer() {}

		virtual std::vector<uint32_t> GetIndices() const = 0;
		virtual uint32_t GetIndexBufferCount() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Refptr<IndexBuffer> Create(const uint32_t* indices, const uint32_t& size);
	};

}