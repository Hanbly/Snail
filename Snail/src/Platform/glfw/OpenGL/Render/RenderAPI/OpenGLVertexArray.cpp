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

	void OpenGLVertexArray::SetInstanceBuffer(const Refptr<VertexBuffer>& vertexBuffer)
	{
		if (m_SettedInstanceBuffer) return;

		Bind();
		vertexBuffer->Bind();

		// 复用 m_VertexBufferIndexOffset，这样实例属性的 index 会紧接在顶点属性之后
		// 例如：Pos(0), Normal(1), Tex(2) -> ModelMatrix 会从 3 开始
		uint32_t& index = m_VertexBufferIndexOffset;

		auto& layout = vertexBuffer->GetLayout();
		auto& elements = layout->GetLayoutElements();

		for (const auto& element : elements)
		{
			// 这里的逻辑需要特殊处理 Matrix 类型
			// OpenGL 中 mat4 实际上占用 4 个连续的 vec4 属性位置
			if (element.type == VertexDataType::Mat4) {
				uint8_t count = 4; // mat4 有 4 列
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(index);
					glVertexAttribPointer(
						index,
						4, // 每一列是 vec4，也就是4个float
						GL_FLOAT,
						element.enableNormalize ? GL_TRUE : GL_FALSE,
						layout->GetLayoutSize(),
						(const void*)(element.offset + sizeof(float) * 4 * i)
					);
					// 关键点：设置属性除数为 1，表示每 1 个实例更新一次数据
					glVertexAttribDivisor(index, 1);
					index++;
				}
			}
			else if (element.type == VertexDataType::Mat3) {
				uint8_t count = 3; // mat3 有 3 列
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(index);
					glVertexAttribPointer(
						index,
						3, // 注意：这里是 3 (vec3)
						GL_FLOAT,
						element.enableNormalize ? GL_TRUE : GL_FALSE,
						layout->GetLayoutSize(),
						// 偏移量计算：基础偏移 + 第 i 列 * 每列 3 个 float * float大小
						(const void*)(element.offset + sizeof(float) * 3 * i)
					);
					glVertexAttribDivisor(index, 1);
					index++;
				}
			}
			else {
				glEnableVertexAttribArray(index);
				glVertexAttribPointer(
					index,
					element.GetComponentCount(),
					GetOpenGLType(element.type),
					element.enableNormalize ? GL_TRUE : GL_FALSE,
					layout->GetLayoutSize(),
					(const void*)(uintptr_t)element.offset
				);
				glVertexAttribDivisor(index, 1);
				index++;
			}
			m_SettedInstanceBuffer = true;
		}

		// 我们不需要像 m_VertexBuffer 那样存起来，因为通常实例 Buffer 是外部每一帧传入的通用Buffer
		// 但为了生命周期安全，如果需要也可以存一个 list
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