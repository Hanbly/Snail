#include "SNLpch.h"

#include "OpenGLVertexArray.h"

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

	OpenGLVertexArray::OpenGLVertexArray()
		: m_VertexBufferIndexOffset(0)
	{
		glGenVertexArrays(1, &m_ArrayId);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_ArrayId);
	}

	const std::vector<Vertex> OpenGLVertexArray::GetVertexBuffer() const {		
		return m_VertexBuffer->GetVertices();
	}

	void OpenGLVertexArray::SetVertexBuffer(const Refptr<VertexBuffer>& vertexBuffer)
	{
		this->Bind();
		vertexBuffer->Bind();

		// 启用layout布局（用户需要提前把布局写入VertexBuffer）
		uint32_t& index = m_VertexBufferIndexOffset;
		auto& layout = vertexBuffer->GetLayout();
		auto& elements = layout->GetLayoutElements();
		SNL_CORE_ASSERT(elements.size(), "OpenGLVertexArray: 添加VertexBuffer错误, 缓冲区布局为空!");
		for (const auto& e : elements) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				e.GetComponentCount(),
				GetOpenGLType(e.type),
				e.enableNormalize ? GL_TRUE : GL_FALSE,
				layout->GetLayoutSize(),				// 重要！不要写成e.size
				(const void*)(uintptr_t)e.offset);
			index++;
		}

		m_VertexBuffer = vertexBuffer;
	}

	void OpenGLVertexArray::SetIndexBuffer(const Refptr<IndexBuffer>& indexBuffer)
	{
		this->Bind();
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_ArrayId);
	}

	void OpenGLVertexArray::Unbind() const
	{
		SNL_PROFILE_FUNCTION();


		glBindVertexArray(0);
	}

}