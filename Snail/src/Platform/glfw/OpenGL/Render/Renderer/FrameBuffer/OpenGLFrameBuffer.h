#pragma once

#include "Snail/Render/Renderer/FrameBuffer/FrameBuffer.h"

namespace Snail {

	class OpenGLFrameBuffer : public FrameBuffer {
	private:
		uint32_t m_RendererId;
		uint32_t m_ColorAttachment;
		uint32_t m_MaskAttachment;
		uint32_t m_DepthAttachment;
		uint32_t m_RenderbufferObjectAttachment;

		FrameBufferSpecification m_Specification;

	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		OpenGLFrameBuffer(const OpenGLFrameBuffer&) = delete;
		OpenGLFrameBuffer& operator=(const OpenGLFrameBuffer&) = delete;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		inline virtual const uint32_t& GetColorAttachment() const override { return m_ColorAttachment; }
		inline virtual const uint32_t& GetMaskAttachment() const override { return m_MaskAttachment; }
		inline virtual const uint32_t& GetDepthAttachment() const override { return m_DepthAttachment; }
		inline virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

		virtual void ReGenerate() override;
		virtual void Resize(const uint32_t& width, const uint32_t& height) override;
		virtual void Delete() override;

		// 接收立方体贴图的一个面，用作下一次的绘制对象
		virtual void SetupTextureCubei(const int index, const uint32_t& rendererId) override;
	};

}