#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/RenderAPI/Buffer/IndexBuffer.h"

namespace Snail {

	class OpenGLIndexBuffer : public IndexBuffer {
	private:
		uint32_t m_BufferId;
		std::vector<uint32_t> m_IndicesData;
		uint32_t m_Count;
	public:
		OpenGLIndexBuffer(const uint32_t* indices, const uint32_t& size);
		~OpenGLIndexBuffer();

		virtual std::vector<uint32_t> GetIndices() const override;
		virtual uint32_t GetIndexBufferCount() const override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
	};

}