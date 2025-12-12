#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/RenderAPI/Buffer/BufferLayout.h"

namespace Snail {

	class OpenGLBufferLayout : public BufferLayout {
	private:
		std::vector<VertexElement> m_Elements;
		uint32_t m_Size;
	public:
		OpenGLBufferLayout(const std::initializer_list<VertexElement>& elements);
		~OpenGLBufferLayout();

		virtual std::vector<VertexElement>& GetLayoutElements() override;
		virtual uint32_t GetLayoutSize() const override;
	};

}