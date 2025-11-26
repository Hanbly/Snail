#pragma once

#include "Snail/Core.h"

namespace Snail {

	class SNAIL_API VertexBuffer {
	protected:
		
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static std::shared_ptr<VertexBuffer> CreateVertexBuffer(float* vertices, uint32_t size);
	};

}