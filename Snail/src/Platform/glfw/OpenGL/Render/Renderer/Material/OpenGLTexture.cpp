#include "SNLpch.h"

#include <stb_image.h>
#include <tinyexr.h>

#include "OpenGLTexture.h"

namespace Snail {

	// 内部辅助函数：垂直翻转浮点图像数据
	static void FlipFloatBufferVertically(float* data, int width, int height, int channels)
	{
		if (!data || width <= 0 || height <= 0) return;

		// 计算每一行的数据大小
		size_t rowFloats = static_cast<size_t>(width) * channels;
		size_t rowBytes = rowFloats * sizeof(float);

		// 临时缓冲区，用于交换行
		std::vector<float> tempRow(rowFloats);

		// 只遍历到高度的一半
		for (int y = 0; y < height / 2; ++y) {
			float* topRow = data + (y * rowFloats);
			float* bottomRow = data + ((height - 1 - y) * rowFloats);

			// 交换内存: Top -> Temp, Bottom -> Top, Temp -> Bottom
			memcpy(tempRow.data(), topRow, rowBytes);
			memcpy(topRow, bottomRow, rowBytes);
			memcpy(bottomRow, tempRow.data(), rowBytes);
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::vector<std::string>& path, const TextureUsage& usage)
		: m_RendererId(0), m_Path(path), m_Type(TextureType::TWOD), m_Usage(usage)
	{
		LoadTexture2D(path);
	}

	OpenGLTexture2D::~OpenGLTexture2D() 
	{
		glDeleteTextures(1, &m_RendererId);
	}

	void OpenGLTexture2D::Bind(const uint32_t& slot) const 
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererId);
	}

	void OpenGLTexture2D::Unbind(const uint32_t& slot) const 
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture2D::BindExternal(const uint32_t& slot, const uint32_t& rendererId)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, rendererId);
	}

	void OpenGLTexture2D::LoadTexture2D(const std::vector<std::string>& path)
	{
		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		void* data = nullptr;
		bool isFloat = false;

		std::filesystem::path filePath(path[0]);
		std::string extension = filePath.extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		if (extension == ".exr") {
			const char* err = nullptr;
			int ret = LoadEXR((float**)&data, &width, &height, path[0].c_str(), &err);

			if (ret != TINYEXR_SUCCESS) {
				if (err) {
					SNL_CORE_ERROR("OpenGLTexture2D: TinyEXR 加载失败: \n路径: {0} \n错误信息: {1}", path[0], err);
					FreeEXRErrorMessage(err); // TinyEXR 要求释放错误信息内存
				}
				else {
					SNL_CORE_ERROR("OpenGLTexture2D: TinyEXR 加载失败: {0} (未知错误)", path[0]);
				}
				return;
			}
			// TinyEXR 加载的总是 RGBA float，除非特殊设置
			channels = 4;
			isFloat = true;

			// 反转y轴
			FlipFloatBufferVertically(static_cast<float*>(data), width, height, channels);
		}
		else {
			// 判断是否为 HDR (stbi 只支持 .hdr，不支持 .exr)
			if (stbi_is_hdr(path[0].c_str())) {
				// 加载为浮点数据
				data = stbi_loadf(path[0].c_str(), &width, &height, &channels, 0);
				isFloat = true;
			}
			else {
				// 普通 LDR 加载 (png, jpg)
				data = stbi_load(path[0].c_str(), &width, &height, &channels, 0);
				isFloat = false;
			}
			if (!data) {
				const char* reason = stbi_failure_reason();
				SNL_CORE_ERROR("OpenGLTexture2D: stbi_load 加载失败!\n路径: {0} \n错误信息: {1}", path[0], reason);
				return;
			}
		}

		m_Width = width;
		m_Height = height;

		GLenum type = GL_UNSIGNED_BYTE;
		GLenum internalFormat = GL_RGB8;
		GLenum dataFormat = GL_RGB;
		if (isFloat) {
			// HDR / EXR 处理
			type = GL_FLOAT;
			if (channels == 1) {
				internalFormat = GL_R32F;
				dataFormat = GL_RED;
			}
			else if (channels == 2) {
				internalFormat = GL_RG32F;
				dataFormat = GL_RG;
			}
			else if (channels == 3) {
				internalFormat = GL_RGB32F;
				dataFormat = GL_RGB;
			}
			else if (channels == 4) {
				internalFormat = GL_RGBA32F;
				dataFormat = GL_RGBA;
			}
		}
		else {
			// 普通 LDR 处理
			type = GL_UNSIGNED_BYTE;
			if (channels == 1) {
				internalFormat = GL_R8;
				dataFormat = GL_RED;
			}
			else if (channels == 2) {
				internalFormat = GL_RG8;
				dataFormat = GL_RG;
			}
			else if (channels == 3) {
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}
			else if (channels == 4) {
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
		}

		if (internalFormat == 0 || dataFormat == 0)
		{
			SNL_CORE_ERROR("纹理格式错误! Channels: {0}, Path: {1}", channels, path);
			if (extension == ".exr") {
				free(data); // TinyEXR 的数据通常由 malloc 分配
			}
			else {
				stbi_image_free(data);
			}
			return;
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

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, type, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		Unbind();
		if (extension == ".exr") {
			free(data); // TinyEXR 的数据通常由 malloc 分配
		}
		else {
			stbi_image_free(data);
		}
	}

	OpenGLTextureCube::OpenGLTextureCube(const int dim)
		: m_RendererId(0), m_Type(TextureType::Cube), m_Usage(TextureUsage::Cubemap)
	{
		glGenTextures(1, &m_RendererId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererId);

		for (unsigned int i = 0; i < 6; ++i)
		{
			// 分配内存
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F,
				dim, dim, 0, GL_RGBA, GL_FLOAT, nullptr);
		}
		// 采样参数
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		m_Width = dim;
		m_Height = dim;
	}

	OpenGLTextureCube::OpenGLTextureCube(const std::vector<std::string>& path, const TextureUsage& usage)
		: m_RendererId(0), m_Path(path), m_Type(TextureType::Cube), m_Usage(usage)
	{
		SNL_PROFILE_FUNCTION();


		if (path.empty()) {
			SNL_CORE_ERROR("OpenGLTextureCube: 路径为空!");
			return;
		}

		LoadCubemapFromFaces(path);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		SNL_PROFILE_FUNCTION();


		glDeleteTextures(1, &m_RendererId);
	}

	void OpenGLTextureCube::Bind(const uint32_t& slot) const
	{
		SNL_PROFILE_FUNCTION();


		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererId);
	}

	void OpenGLTextureCube::Unbind(const uint32_t& slot) const
	{
		SNL_PROFILE_FUNCTION();


		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void OpenGLTextureCube::LoadCubemapFromFaces(const std::vector<std::string>& path)
	{
		// Cubemap 通常不需要翻转 Y 轴，保持 false
		stbi_set_flip_vertically_on_load(false);

		glGenTextures(1, &m_RendererId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererId);

		// 遍历 6 个面
		for (int i = 0; i < 6; i++) {
			int width, height, channels;
			void* data = nullptr;
			bool isFloat = false; // 标记是否为浮点数据 (HDR/EXR)

			std::filesystem::path filePath(path[i]);
			std::string extension = filePath.extension().string();
			// 转小写
			std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

			// ------------------ 加载数据 (EXR / HDR / LDR) ------------------
			if (extension == ".exr") {
				const char* err = nullptr;
				int ret = LoadEXR((float**)&data, &width, &height, path[i].c_str(), &err);

				if (ret != TINYEXR_SUCCESS) {
					if (err) {
						SNL_CORE_ERROR("OpenGLTextureCube: TinyEXR 加载失败: {0} \n{1}", path[i], err);
						FreeEXRErrorMessage(err);
					}
					else {
						SNL_CORE_ERROR("OpenGLTextureCube: TinyEXR 加载失败: {0}", path[i]);
					}
					return;
				}
				channels = 4; // TinyEXR 默认通常加载为 RGBA
				isFloat = true;

				// 反转y轴
				FlipFloatBufferVertically(static_cast<float*>(data), width, height, channels);
			}
			else {
				// 检测是否为 HDR 格式
				if (stbi_is_hdr(path[i].c_str())) {
					data = stbi_loadf(path[i].c_str(), &width, &height, &channels, 0);
					isFloat = true;
				}
				else {
					// 普通图片 (png, jpg等)
					data = stbi_load(path[i].c_str(), &width, &height, &channels, 0);
					isFloat = false;
				}

				if (!data) {
					SNL_CORE_ERROR("OpenGLTextureCube: stbi 加载失败! Path: {0}", path[i]);
					return;
				}
			}

			m_Width = width;
			m_Height = height;

			// ------------------ 确定 OpenGL 格式 ------------------
			GLenum type = isFloat ? GL_FLOAT : GL_UNSIGNED_BYTE;
			GLenum internalFormat = 0;
			GLenum dataFormat = 0;

			if (isFloat) {
				// --- 浮点纹理 (HDR/EXR) ---
				if (channels == 1) {
					internalFormat = GL_R32F;
					dataFormat = GL_RED;
				}
				else if (channels == 2) {
					internalFormat = GL_RG32F;
					dataFormat = GL_RG;
				}
				else if (channels == 3) {
					internalFormat = GL_RGB32F;
					dataFormat = GL_RGB;
				}
				else if (channels == 4) {
					internalFormat = GL_RGBA32F;
					dataFormat = GL_RGBA;
				}
			}
			else {
				// --- 普通字节纹理 (LDR) ---
				if (channels == 1) {
					internalFormat = GL_R8;
					dataFormat = GL_RED;
				}
				else if (channels == 2) {
					internalFormat = GL_RG8;
					dataFormat = GL_RG;
				}
				else if (channels == 3) {
					internalFormat = GL_RGB8;
					dataFormat = GL_RGB;
				}
				else if (channels == 4) {
					internalFormat = GL_RGBA8;
					dataFormat = GL_RGBA;
				}
			}

			// 校验格式
			if (internalFormat == 0 || dataFormat == 0) {
				SNL_CORE_ERROR("纹理通道数不支持! Channels: {0}, Path: {1}", channels, path[i]);
				if (extension == ".exr") free(data); // TinyEXR 使用 free
				else stbi_image_free(data);          // STB 使用 stbi_image_free
				continue;
			}

			// ------------------ 上传纹理数据 ------------------
			// 设置对齐，防止非4字节对齐导致的崩溃
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				internalFormat,
				m_Width, m_Height,
				0,
				dataFormat,
				type,
				data
			);

			if (extension == ".exr") {
				free(data); // TinyEXR 的数据通常由 malloc 分配
			}
			else {
				stbi_image_free(data);
			}
		}

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// 如果使用了 Mipmap，MinFilter 需要改为 Mipmap 模式，否则改为 GL_LINEAR
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

}