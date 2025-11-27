#include "SNLpch.h"

#include "OpenGLIndexBuffer.h"

namespace Snail {

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t size)
		: m_Count(0)
	{
		SNL_CORE_ASSERT(indices, "OpenGLIndexBuffer: 数据为空, 初始化失败!");

		glGenBuffers(1, &m_BufferId);
		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);

		m_Count = size / sizeof(uint32_t);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferId);
	}

	uint32_t OpenGLIndexBuffer::GetIndexBufferCount() const
	{
		return m_Count;
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}