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

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		this->Bind();
		vertexBuffer->Bind();

		// 启用layout布局（用户需要提前把布局写入VertexBuffer）
		uint32_t& index = m_VertexBufferIndexOffset;
		auto& layout = vertexBuffer->GetLayout();
		auto& elements = layout->GetLayoutElements();
		for (const auto& e : elements) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				e.GetComponentCount(),
				GetOpenGLType(e.type),
				e.enableNormalize ? GL_TRUE : GL_FALSE, // 显式转为 GL boolean
				layout->GetLayoutSize(),
				(const void*)(uintptr_t)e.offset);		// 规范的 Offset 转换
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
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
		glBindVertexArray(0);
	}

}