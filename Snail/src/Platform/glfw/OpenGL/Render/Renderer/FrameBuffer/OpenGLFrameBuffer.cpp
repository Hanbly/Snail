#include "SNLpch.h"

#include "OpenGLFrameBuffer.h"

namespace Snail {

	static const uint32_t MaxFrameBufferSize = 8192;

	// 工具函数：判断是否是深度格式
	static bool IsDepthFormat(FrameBufferTextureFormat format) {
		switch (format) {
		case FrameBufferTextureFormat::DEPTH_COMPONENT: return true;
		case FrameBufferTextureFormat::DEPTH24_STENCIL8: return true;
		}
		return false;
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
		: m_RendererId(0), m_ColorAttachments({}), m_MaskAttachment(0), m_DepthAttachment(0), m_RenderbufferObjectAttachment(0), m_Specification(spec)
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

		bool multisample = false; // 暂时还没支持多重采样，预留标记

		// 1. 遍历所有附件规范，根据类型分别处理
		for (auto& attachmentSpec : m_Specification.attachments)
		{
			if (IsDepthFormat(attachmentSpec.TextureFormat)) {
				// ------------- 创建深度附件 -------------
				glGenTextures(1, &m_DepthAttachment);
				glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

				switch (attachmentSpec.TextureFormat) {
				case FrameBufferTextureFormat::DEPTH_COMPONENT:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Specification.width, m_Specification.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
					// 使用 CLAMP_TO_BORDER
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
					// 设置边界颜色为纯白 (深度 = 1.0f)
					// 这意味着：超出范围的地方，深度都是最大值，因此永远不会被判定为阴影
					float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
					glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
					break;
				}
				case FrameBufferTextureFormat::DEPTH24_STENCIL8:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.width, m_Specification.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
					// 深度图通常不需要重复
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
					break;
				}
				}
			}
			else
			{
				// ------------- 创建颜色附件 -------------
				uint32_t tempTextureID;
				glGenTextures(1, &tempTextureID);
				glBindTexture(GL_TEXTURE_2D, tempTextureID);

				GLenum internalFormat = GL_RGBA8;
				GLenum dataFormat = GL_RGBA;
				GLenum dataType = GL_UNSIGNED_BYTE;
				GLenum filter = GL_LINEAR; // 默认线性过滤
				GLenum wrap = GL_CLAMP_TO_EDGE;

				switch (attachmentSpec.TextureFormat)
				{
				case FrameBufferTextureFormat::RGB8:
					internalFormat = GL_RGB8; dataFormat = GL_RGB; dataType = GL_UNSIGNED_BYTE; break;
				case FrameBufferTextureFormat::RGB16F:
					internalFormat = GL_RGB16F; dataFormat = GL_RGB; dataType = GL_FLOAT; break;
				case FrameBufferTextureFormat::RGB32F:
					internalFormat = GL_RGB32F; dataFormat = GL_RGB; dataType = GL_FLOAT; break;
				case FrameBufferTextureFormat::RGBA8:
					internalFormat = GL_RGBA8; dataFormat = GL_RGBA; dataType = GL_UNSIGNED_BYTE; break;
				case FrameBufferTextureFormat::RGBA16F:
					internalFormat = GL_RGBA16F; dataFormat = GL_RGBA; dataType = GL_FLOAT; break;
				case FrameBufferTextureFormat::RGBA32F:
					internalFormat = GL_RGBA32F; dataFormat = GL_RGBA; dataType = GL_FLOAT; break;
				case FrameBufferTextureFormat::R32I:
					internalFormat = GL_R32I;
					dataFormat = GL_RED_INTEGER;
					dataType = GL_INT;
					filter = GL_NEAREST; // 整数纹理必须最近邻
					break;
				}

				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Specification.width, m_Specification.height, 0, dataFormat, dataType, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

				// 使用当前的 vector 大小作为 Attachment Index (0, 1, 2...)
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (int)m_ColorAttachments.size(), GL_TEXTURE_2D, tempTextureID, 0);

				// 将 ID 存入 vector
				m_ColorAttachments.push_back(tempTextureID);
			}
		}

		// --- 设置 Draw Buffers ---
		if (m_ColorAttachments.size() > 0) {
			// 只有深度图时不需要 DrawBuffers，有颜色图时动态生成
			std::vector<GLenum> buffers;
			for (unsigned int i = 0; i < m_ColorAttachments.size(); i++)
				buffers.push_back(GL_COLOR_ATTACHMENT0 + i);

			// 只开启实际存在的数量
			glDrawBuffers((GLsizei)m_ColorAttachments.size(), buffers.data());
		}
		else if (m_ColorAttachments.empty()) {
			// 只有深度附件 (Shadow Map)
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		// ------- 检查完整性和解绑 --------
		SNL_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "OpenGLFrameBuffer: 帧缓冲附件不完整!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(const uint32_t& width, const uint32_t& height)
	{
		if (width <= 0 || height <= 0 || width > MaxFrameBufferSize || height > MaxFrameBufferSize) {
			return;
		}

		m_Specification.width = width;
		m_Specification.height = height;

		ReGenerate();
	}

	void OpenGLFrameBuffer::Delete()
	{
		if (m_RendererId)
		{
			glDeleteFramebuffers(1, &m_RendererId);
			// 只有 vector 不为空时才删除
			if (!m_ColorAttachments.empty()) {
				glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
			}
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
			m_RendererId = 0;
		}
	}

	void OpenGLFrameBuffer::SetupTextureCubei(const int index, const uint32_t& rendererId, const int mip)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, rendererId, mip);
	}

}