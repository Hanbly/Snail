#include "SNLpch.h"

#include <stb_image.h>
#include <tinyexr.h>

#include "OpenGLTexture.h"

namespace Snail {

	// 处理纹理数据：从 -1~1 映射到 0~1
	//static void ProcessEXRData(float* data, int width, int height, int channels, Snail::TextureUsage usage)
	//{
	//	if (!data || width <= 0 || height <= 0) return;

	//	size_t pixelCount = static_cast<size_t>(width) * height;

	//	// 遍历所有像素
	//	for (size_t i = 0; i < pixelCount; ++i) {
	//		float* pixel = data + (i * channels); // 获取当前像素指针 (R, G, B, A)

	//		if (usage == Snail::TextureUsage::Normal)
	//		{
	//			// 法线贴图通常是 [-1, 1]，需要映射到 [0, 1] 才能正确作为纹理采样或可视化
	//			// 算法: pixel = pixel * 0.5 + 0.5
	//			// 只处理 RGB，忽略 Alpha
	//			pixel[0] = pixel[0] * 0.5f + 0.5f;
	//			pixel[1] = pixel[1] * 0.5f + 0.5f;
	//			pixel[2] = pixel[2] * 0.5f + 0.5f;
	//		}
	//		else if (usage == Snail::TextureUsage::Roughness || usage == Snail::TextureUsage::Metallic)
	//		{
	//			// 粗糙度/金属度 必须严格在 [0, 1] 之间
	//			// 有些 EXR 可能会有超出的 HDR 值，导致渲染异常，这里进行 Clamp 操作
	//			for (int c = 0; c < 3; ++c) { // 如果是灰度图存成了RGB，处理前3个通道
	//				if (pixel[c] < 0.0f) pixel[c] = 0.0f;
	//				if (pixel[c] > 1.0f) pixel[c] = 1.0f;
	//			}
	//		}
	//	}
	//}

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
		if (m_UIId > 0) glDeleteTextures(1, &m_UIId);
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
			// 设置通道数量
			channels = 4;
			isFloat = true;

			// 根据用途预处理数据范围
			//ProcessEXRData(static_cast<float*>(data), width, height, channels, m_Usage);

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
			internalFormat = GL_RGBA32F;
			dataFormat = GL_RGBA;
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
				// 如果是颜色贴图，使用 sRGB
				// 注意：只有当这张图是"人眼看的颜色"(Albedo/Diffuse)时才用 sRGB
				// 如果是 法线(Normal)、粗糙度(Roughness)等数据图，必须保持 GL_RGB8
				if (m_Usage == TextureUsage::Diffuse || m_Usage == TextureUsage::Cubemap || m_Usage == TextureUsage::Albedo) {
					internalFormat = GL_SRGB8;
				}
				else {
					internalFormat = GL_RGB8;
				}
				dataFormat = GL_RGB;
			}
			else if (channels == 4) {
				if (m_Usage == TextureUsage::Diffuse || m_Usage == TextureUsage::Cubemap || m_Usage == TextureUsage::Albedo) {
					internalFormat = GL_SRGB8_ALPHA8;
				}
				else {
					internalFormat = GL_RGBA8;
				}
				dataFormat = GL_RGBA;
			}
		}

		bool useSRGB = (internalFormat == GL_SRGB8 || internalFormat == GL_SRGB8_ALPHA8);

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

		// 防止单通道(灰度)图片宽度不是4的倍数时导致倾斜/错位
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// 计算 Mipmap 层级数量
		int levels = 1;
		if (m_Width > 0 && m_Height > 0)
			levels = 1 + floor(log2(std::max(m_Width, m_Height)));

		// 分配不可变显存 (Immutable Storage)
		glTexStorage2D(GL_TEXTURE_2D, levels, internalFormat, m_Width, m_Height);

		// 上传数据
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, dataFormat, type, data);

		// 如果是单通道 (GL_RED)，强制 RGB 通道都读取 R 值，Alpha 设为 1
		// 这样 Shader 中读取 .r, .g, .b 都会得到正确的值，预览也是灰度而非红色
		if (dataFormat == GL_RED) {
			GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
			glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
		}

		// 设置参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 生成 Mipmap
		glGenerateMipmap(GL_TEXTURE_2D);

		// ------------ 如果使用了sRGB转换线性，创建UI视图 ---------------
		if (useSRGB) {
			glGenTextures(1, &m_UIId);

			// 确定 View 的格式 (把 SRGB 剥离，只看作纯数据)
			GLenum viewFormat = (internalFormat == GL_SRGB8) ? GL_RGB8 : GL_RGBA8;

			// glTextureView(新ID, 类型, 原ID, 格式, Mip层级范围...)
			glTextureView(m_UIId, GL_TEXTURE_2D, m_RendererId, viewFormat, 0, 1000, 0, 1);

			// 注意：TextureView 需要设置自己的采样器参数
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_UIId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0); // 解绑
		}

		Unbind();
		if (extension == ".exr") {
			free(data); // TinyEXR 的数据通常由 malloc 分配
		}
		else {
			stbi_image_free(data);
		}
	}

	OpenGLTextureCube::OpenGLTextureCube(const int dim, const bool mipmap)
		: m_RendererId(0), m_Type(TextureType::Cube), m_Usage(TextureUsage::Cubemap)
	{
		glGenTextures(1, &m_RendererId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererId);

		m_Width = dim;
		m_Height = dim;

		// 计算需要的 Mipmap 层级数
		int levels = 1;
		if (mipmap) {
			// 公式：floor(log2(max(w, h))) + 1
			levels = static_cast<int>(std::floor(std::log2(dim))) + 1;
		}

		glTexStorage2D(GL_TEXTURE_CUBE_MAP, levels, GL_RGBA32F, dim, dim);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// 如果启用了 mipmap，Min Filter 必须改为支持 mipmap 的模式
		if (mipmap) {
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else {
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
		glDeleteTextures(1, &m_RendererId);
		if (m_UIId > 0) glDeleteTextures(1, &m_UIId);
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

		GLenum internalFormat = 0; // 需要在循环外记录格式供View使用
		int levels = 1;            // Mipmap层级
		bool storageAllocated = false;

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
				//FlipFloatBufferVertically(static_cast<float*>(data), width, height, channels);
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

			// ------------------ 确定 OpenGL 格式 ------------------
			GLenum type = isFloat ? GL_FLOAT : GL_UNSIGNED_BYTE;
			// 注意：这里定义局部变量用于当前计算，最终会存入外部 internalFormat
			GLenum currentInternalFormat = 0;
			GLenum dataFormat = 0;

			if (isFloat) {				
				currentInternalFormat = GL_RGBA32F;
				dataFormat = GL_RGBA;
			}
			else {
				// --- 普通字节纹理 (LDR) ---
				if (channels == 3) {
					if (m_Usage == TextureUsage::Diffuse || m_Usage == TextureUsage::Cubemap) {
						currentInternalFormat = GL_SRGB8;
					}
					else {
						currentInternalFormat = GL_RGB8;
					}
					dataFormat = GL_RGB;
				}
				else if (channels == 4) {
					if (m_Usage == TextureUsage::Diffuse || m_Usage == TextureUsage::Cubemap) {
						currentInternalFormat = GL_SRGB8_ALPHA8;
					}
					else {
						currentInternalFormat = GL_RGBA8;
					}
					dataFormat = GL_RGBA;
				}
			}

			// 校验格式
			if (currentInternalFormat == 0 || dataFormat == 0) {
				SNL_CORE_ERROR("纹理通道数不支持! Channels: {0}, Path: {1}", channels, path[i]);
				if (extension == ".exr") free(data);
				else stbi_image_free(data);
				continue;
			}

			// ------------------ 上传纹理数据 (使用 Immutable Storage) ------------------

			// 如果是第一张图，初始化不可变存储
			if (!storageAllocated) {
				m_Width = width;
				m_Height = height;
				internalFormat = currentInternalFormat;

				// 计算 mipmap 层级
				levels = 1;
				if (m_Width > 0 && m_Height > 0)
					levels = 1 + floor(log2(std::max(m_Width, m_Height)));

				glTexStorage2D(GL_TEXTURE_CUBE_MAP, levels, internalFormat, m_Width, m_Height);
				storageAllocated = true;
			}

			// 设置对齐，防止非4字节对齐导致的崩溃
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			// 使用 SubImage 上传
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, 0, 0, m_Width, m_Height,
				dataFormat, type, data);

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

		// ------------ 如果使用了sRGB转换线性，创建UI视图 (循环外执行) ---------------
		bool useSRGB = (internalFormat == GL_SRGB8 || internalFormat == GL_SRGB8_ALPHA8);

		if (useSRGB) {
			glGenTextures(1, &m_UIId);

			// 确定 View 的格式 (把 SRGB 剥离，只看作纯数据)
			GLenum viewFormat = (internalFormat == GL_SRGB8) ? GL_RGB8 : GL_RGBA8;

			// glTextureView(新ID, 类型, 原ID, 格式, Mip层级范围, 图层范围)
			// Cubemap 有 6 个 Layer
			glTextureView(m_UIId, GL_TEXTURE_CUBE_MAP, m_RendererId, viewFormat, 0, levels, 0, 6);

			// 注意：TextureView 需要设置自己的采样器参数
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_UIId);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

}