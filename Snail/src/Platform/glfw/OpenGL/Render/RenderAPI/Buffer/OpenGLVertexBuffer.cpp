#include "SNLpch.h"

#include "OpenGLVertexBuffer.h"

namespace Snail {

	OpenGLVertexBuffer::OpenGLVertexBuffer(const uint32_t& size)
		: m_Size(size)
	{
		glGenBuffers(1, &m_BufferId);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

		m_VerticesData.reserve(size);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, const uint32_t& size)
		: m_Size(size)
	{
		SNL_CORE_ASSERT(vertices, "OpenGLVertexBuffer: 数据为空, 初始化失败!");

		glGenBuffers(1, &m_BufferId);
		Bind();
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

		// 申请内存空间，保存原始数据而非指针
		m_VerticesData.resize(size);
		memcpy(m_VerticesData.data(), vertices, size);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_BufferId);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
		// 使用 glBufferSubData 更新部分或全部缓冲数据
		// 注意：size 不能超过创建时的容量
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	std::vector<Vertex> OpenGLVertexBuffer::GetVertices() const
	{
		if (m_VerticesData.empty()) {
			return std::vector<Vertex>();
		}

		const Vertex* start = reinterpret_cast<const Vertex*>(m_VerticesData.data());
		size_t count = m_Size / sizeof(Vertex);

		return std::vector<Vertex>(start, start + count);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}