#pragma once

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

#include "Snail/Render/RenderAPI/Buffer/BufferLayout.h"

namespace Snail {

	class SNAIL_API VertexBuffer {
	public:
		virtual ~VertexBuffer() {}

		virtual Refptr<BufferLayout> GetLayout() const = 0;
		virtual void SetLayout(const Refptr<BufferLayout>& layout) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Refptr<VertexBuffer> Create(float* vertices, uint32_t size);
	};

}