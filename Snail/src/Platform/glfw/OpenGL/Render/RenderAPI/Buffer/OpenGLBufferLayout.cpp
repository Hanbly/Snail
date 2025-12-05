#include "SNLpch.h"

#include "OpenGLBufferLayout.h"

namespace Snail {

	OpenGLBufferLayout::OpenGLBufferLayout(const std::initializer_list<VertexElement>& elements)
		: m_Size(0)
	{
		SNL_PROFILE_FUNCTION();


		m_Elements = elements;
		SNL_CORE_ASSERT(m_Elements.data(), "OpenGLBufferLayout: 布局构造错误, 布局数据为空!");

		for (VertexElement& element : m_Elements) {
			// 某元素的offset是之前所有元素的大小之和
			element.offset = m_Size;
			m_Size += element.size;
		}
	}

	OpenGLBufferLayout::~OpenGLBufferLayout()
	{
	}

	std::vector<VertexElement>& OpenGLBufferLayout::GetLayoutElements() 
	{
		return m_Elements;
	}

	uint32_t OpenGLBufferLayout::GetLayoutSize() const
	{
		return m_Size;
	}

}