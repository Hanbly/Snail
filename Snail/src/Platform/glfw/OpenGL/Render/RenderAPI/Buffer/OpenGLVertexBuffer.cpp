#include "SNLpch.h"

#include "OpenGLVertexBuffer.h"

namespace Snail {

	static GLenum GetOpenGLType(VertexDataType type) {
		switch (type) {
		case VertexDataType::None: SNL_CORE_ASSERT(false, "struct VertexElement: 不支持的类型None!");
		case VertexDataType::Float:		return GL_FLOAT;
		case VertexDataType::Float2:	return GL_FLOAT;
		case VertexDataType::Float3:	return GL_FLOAT;
		case VertexDataType::Float4:	return GL_FLOAT;
		case VertexDataType::Int:		return GL_INT;
		case VertexDataType::Int2:		return GL_INT;
		case VertexDataType::Int3:		return GL_INT;
		case VertexDataType::Int4:		return GL_INT;
		case VertexDataType::Mat2:		return GL_FLOAT;
		case VertexDataType::Mat3:		return GL_FLOAT;
		case VertexDataType::Mat4:		return GL_FLOAT;
		case VertexDataType::Bool:		return GL_BOOL;
		default: SNL_CORE_ASSERT(false, "struct VertexElement: 未知顶点元素的类型!");
		}
		return 0;
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		SNL_CORE_ASSERT(vertices, "OpenGLVertexBuffer: 数据为空, 初始化失败!");

		glGenBuffers(1, &m_BufferId);
		Bind();
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_BufferId);
	}

	void OpenGLVertexBuffer::SetLayout(const std::shared_ptr<BufferLayout>& layout)
	{
		SNL_CORE_ASSERT(layout->GetLayoutElements().data(), "OpenGLVertexBuffer: Layout数据为空, 保存布局失败!");
		m_Layout = layout;
	}

	void OpenGLVertexBuffer::EnableLayout(bool enableNormalize)
	{
		uint8_t index = 0;
		for (const VertexElement& e : m_Layout->GetLayoutElements()) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index, 
				e.GetComponentCount(), 
				GetOpenGLType(e.type), 
				enableNormalize, 
				m_Layout->GetLayoutSize(), 
				(const char*)e.offset);
			index++;
		}
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