#pragma once

#include "Snail/Core.h"

#include "Snail/Render/RenderAPI/IndexBuffer.h"

namespace Snail {

	class OpenGLIndexBuffer : public IndexBuffer {
	private:
		uint32_t m_BufferId;
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;
	};

}