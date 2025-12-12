#pragma once

#include "Snail/Render/Renderer/FrameBuffer/FrameBuffer.h"

namespace Snail {

	class OpenGLFrameBuffer : public FrameBuffer {
	private:
		uint32_t m_RendererId;
		uint32_t m_ColorAttachment;
		uint32_t m_DepthAttachment;
		uint32_t m_RenderbufferObjectAttachment;

		FrameBufferSpecification m_Specification;

	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		inline virtual const uint32_t& GetColorAttachment() const override { return m_ColorAttachment; }
		inline virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

		virtual void Generate() override;
	};

}