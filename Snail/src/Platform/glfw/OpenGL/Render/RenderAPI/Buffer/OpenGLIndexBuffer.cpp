#include "SNLpch.h"

#include "OpenGLIndexBuffer.h"

namespace Snail {

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, const uint32_t& size)
		: m_Count(0)
	{
		SNL_CORE_ASSERT(indices, "OpenGLIndexBuffer: 数据为空, 初始化失败!");

		glGenBuffers(1, &m_BufferId);
		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);

		m_Count = size / sizeof(uint32_t);

		// 申请内存空间，保存原始数据而非指针
		m_IndicesData.resize(m_Count);
		memcpy(m_IndicesData.data(), indices, size);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferId);
	}

	std::vector<uint32_t> OpenGLIndexBuffer::GetIndices() const
	{
		return m_IndicesData;
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