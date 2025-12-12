#include "SNLpch.h"

#include "OpenGLFrameBuffer.h"

namespace Snail {

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
		: m_RendererId(0), m_ColorAttachment(0), m_DepthAttachment(0), m_RenderbufferObjectAttachment(0), m_Specification(spec)
	{
		ReGenerate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		Delete();
	}

	void OpenGLFrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);

		glViewport(0, 0, m_Specification.width, m_Specification.height);
	}

	void OpenGLFrameBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::ReGenerate()
	{
		if (m_RendererId) {
			Delete();
		}

		// 重新生成
		glGenFramebuffers(1, &m_RendererId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);

		// --------------------- ColorAttachment -----------------------
		glGenTextures(1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.width, m_Specification.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		//// --------------------- DepthAttachment -----------------------
		//glGenTextures(1, &m_DepthAttachment);
		//glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

		//glTexImage2D(
		//	GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.width, m_Specification.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		// --------------------- RenderbufferObjectAttachment -----------------------
		glGenRenderbuffers(1, &m_RenderbufferObjectAttachment);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferObjectAttachment);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Specification.width, m_Specification.height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferObjectAttachment);


		SNL_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "OpenGLFrameBuffer: 帧缓冲附件不完整!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(const uint32_t& width, const uint32_t& height)
	{
		m_Specification.width = width;
		m_Specification.height = height;

		ReGenerate();
	}

	void OpenGLFrameBuffer::Delete()
	{
		glDeleteTextures(1, &m_ColorAttachment);
		//glDeleteTextures(1, &m_DepthAttachment);
		glDeleteRenderbuffers(1, &m_RenderbufferObjectAttachment);

		glDeleteFramebuffers(1, &m_RendererId);
	}

}