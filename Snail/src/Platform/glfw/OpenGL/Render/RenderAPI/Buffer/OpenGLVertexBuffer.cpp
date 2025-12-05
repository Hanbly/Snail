#include "SNLpch.h"

#include "OpenGLVertexBuffer.h"

namespace Snail {

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		SNL_PROFILE_FUNCTION();


		SNL_CORE_ASSERT(vertices, "OpenGLVertexBuffer: 数据为空, 初始化失败!");

		glGenBuffers(1, &m_BufferId);
		Bind();
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		SNL_PROFILE_FUNCTION();


		glDeleteBuffers(1, &m_BufferId);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		SNL_PROFILE_FUNCTION();


		glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		SNL_PROFILE_FUNCTION();


		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}