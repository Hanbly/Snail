#pragma once

#include "Snail/Render/Renderer/FrameBuffer/FrameBuffer.h"

namespace Snail {

	class OpenGLFrameBuffer : public FrameBuffer {
	private:
		uint32_t m_RendererId;
		std::vector<uint32_t> m_ColorAttachments;
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

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override {
			if (index >= m_ColorAttachments.size()) return 0; // 越界检查
			return m_ColorAttachments[index];
		}
		virtual uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }
		inline virtual const FrameBufferSpecification& GetSpecification() const override { return m_Specification; }

		virtual void ReGenerate() override;
		virtual void Resize(const uint32_t& width, const uint32_t& height) override;
		virtual void Delete() override;

		// 接收立方体贴图的一个面，用作下一次的绘制对象
		virtual void SetupTextureCubei(const int index, const uint32_t& rendererId, const int mip = 0) override;
	};

}