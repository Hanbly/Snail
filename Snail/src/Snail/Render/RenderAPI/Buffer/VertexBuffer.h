#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/RenderAPI/Buffer/BufferLayout.h"

#include "glm/glm.hpp"

namespace Snail {

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
	};

	class VertexBuffer {
	public:
		virtual ~VertexBuffer() {}

		virtual Refptr<BufferLayout> GetLayout() const = 0;
		virtual void SetLayout(const Refptr<BufferLayout>& layout) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Refptr<VertexBuffer> Create(const void* vertices, const uint32_t& size);
	};

}