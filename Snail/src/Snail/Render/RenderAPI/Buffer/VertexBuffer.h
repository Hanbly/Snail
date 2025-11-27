#pragma once

#include "Snail/Core.h"

#include "Snail/Render/RenderAPI/Buffer/BufferLayout.h"

namespace Snail {

	class SNAIL_API VertexBuffer {
	public:
		virtual ~VertexBuffer() {}

		virtual std::shared_ptr<BufferLayout> GetLayout() const = 0;
		virtual void SetLayout(const std::shared_ptr<BufferLayout>& layout) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static std::shared_ptr<VertexBuffer> CreateVertexBuffer(float* vertices, uint32_t size);
	};

}