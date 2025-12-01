#pragma once

#include "Snail/Core/Core.h"

#include "Snail/Render/RenderAPI/Buffer/IndexBuffer.h"

namespace Snail {

	class OpenGLIndexBuffer : public IndexBuffer {
	private:
		uint32_t m_BufferId;
		uint32_t m_Count;
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		~OpenGLIndexBuffer();

		virtual uint32_t GetIndexBufferCount() const override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
	};

}