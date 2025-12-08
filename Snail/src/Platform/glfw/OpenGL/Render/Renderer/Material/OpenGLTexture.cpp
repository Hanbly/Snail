#include "SNLpch.h"

#include "stb_image.h"

#include "OpenGLTexture.h"

namespace Snail {

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_RendererId(0), m_Path(path)
	{
		SNL_PROFILE_FUNCTION();


		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		SNL_CORE_ASSERT(data, "OpenGLTexture2D: stbi_load加载纹理文件失败!")
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = GL_RGB8;
		GLenum dataFormat = GL_RGB;
		if (channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		}
		else if (channels == 1) // 高光图往往是单通道灰度图！
		{
			internalFormat = GL_R8;
			dataFormat = GL_RED;
		}

		if (internalFormat == 0 || dataFormat == 0)
		{
			SNL_CORE_ASSERT(false, "纹理格式错误! Channels: {0}, Path: {1}", channels, path);
			stbi_image_free(data);
			return; // 或者抛出异常
		}

		glGenTextures(1, &m_RendererId);
		Bind(0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// 放大使用线性取色
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// 缩小使用临近取色
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // u%方向超出0-1的处理
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // v%方向超出0-1的处理

		// ------------ 内存对齐 ------------
		// 告诉 OpenGL 哪怕是 1 字节对齐也可以
		// 防止宽度不是 4 的倍数时读取越界崩溃
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		// ---------------------------------

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		Unbind();
		stbi_image_free(data);

	}

	OpenGLTexture2D::~OpenGLTexture2D() 
	{
		SNL_PROFILE_FUNCTION();


		glDeleteTextures(1, &m_RendererId);
	}

	void OpenGLTexture2D::Bind(const uint32_t& slot) const 
	{
		SNL_PROFILE_FUNCTION();


		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererId);
	}

	void OpenGLTexture2D::Unbind(const uint32_t& slot) const 
	{
		SNL_PROFILE_FUNCTION();


		glBindTexture(GL_TEXTURE_2D, 0);
	}

}