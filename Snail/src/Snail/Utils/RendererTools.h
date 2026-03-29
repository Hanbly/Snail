#pragma once

#include "Snail/Render/RenderAPI/Buffer/VertexBuffer.h"
#include "Snail/Render/RenderAPI/VertexArray.h"
#include "Snail/Render/Renderer/FrameBuffer/FrameBuffer.h"
#include "Snail/Render/Renderer/Material/Texture.h"
#include "Snail/Render/Renderer/Material/ShaderLibrary.h"
#include "Snail/Render/Renderer/Material/TextureLibrary.h"

#include "Snail/Render/Renderer/RendererCommand.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Snail {

	// 捕获用的投影矩阵 (90度视野, 1:1宽高比)
	static glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	// 捕获用的 6 个观察矩阵
	static glm::mat4 captureViews[] = {
		glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	static Refptr<VertexArray> s_CubeVAO = nullptr;

	class RendererTools {
	public:

		// --- 处理绝对路径 ---
		static std::string CleanFilePath(const std::string& rawPath)
		{
			// --- 路径处理核心 ---
			std::string cleanPathStr;

			std::filesystem::path p(rawPath);
			std::filesystem::path currentPath = std::filesystem::current_path();

			std::filesystem::path relativePath;
			try {
				// 相对于工程目录的路径
				relativePath = std::filesystem::relative(p, currentPath);
			}
			catch (...) {
				// 如果不在同一磁盘，relative 会失败，回退到原始路径
				relativePath = rawPath;
			}

			cleanPathStr = relativePath.string();

			return cleanPathStr;
		}

		static std::string CleanWindowsPath(const std::string& path)
		{
			std::filesystem::path rawPath = path;
			std::filesystem::path finalPath;
			// 遍历路径的每一段
			for (const auto& part : rawPath)
			{
				// 如果这一段是 ".." 或者 "."，就跳过
				if (part == ".." || part == ".") {
					continue;
				}

				// 剩下的部分拼接起来 (会自动处理 / 或 \)
				// 如果 result 是空的，直接赋值；否则用 /= 拼接
				finalPath /= part;
			}

			// 统一将 Windows 的 '\' 替换为 '/'
			std::string result = finalPath.string();
			std::replace(result.begin(), result.end(), '\\', '/');

			return result;
		}

		// --- 手动计算切线空间 ---
		static void RecalculateTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		{
			// 1. 重置切线 & 防御零法线
			for (auto& v : vertices) {
				v.tangent = glm::vec3(0.0f);
				v.bitangent = glm::vec3(0.0f);
				if (glm::length(v.normal) < 0.0001f) {
					v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
				}
			}

			// 2. 遍历三角形累加切线
			for (size_t i = 0; i < indices.size(); i += 3) {

				if(i + 2 >= indices.size() || indices[i] >= vertices.size() || indices[i + 1] >= vertices.size() || indices[i + 2] >= vertices.size()) {
					SNL_CORE_WARN("RendererTools::RecalculateTangents 索引越界，跳过三角形 i={0}, indices={1}, {2}, {3}, vertices size={4}", i, indices[i], indices[i + 1], indices[i + 2], vertices.size());
					continue;
				}

				Vertex& v0 = vertices[indices[i]];
				Vertex& v1 = vertices[indices[i + 1]];
				Vertex& v2 = vertices[indices[i + 2]];

				glm::vec3 edge1 = v1.position - v0.position;
				glm::vec3 edge2 = v2.position - v0.position;

				glm::vec2 deltaUV1 = v1.texCoords - v0.texCoords;
				glm::vec2 deltaUV2 = v2.texCoords - v0.texCoords;

				float denom = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

				glm::vec3 faceTangent(0.0f);
				glm::vec3 faceBitangent(0.0f);

				// 防御极小值，避免产生巨大的乘数 f
				if (std::abs(denom) > 1e-6f) {
					float f = 1.0f / denom;
					faceTangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
					faceBitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);
				}
				else {
					// 如果 UV 畸变或没有 UV，直接用模型边作为切线空间（作为防崩底线）
					faceTangent = edge1;
					faceBitangent = edge2;
				}

				// 在累加到顶点之前，必须先归一化面切线！
				// 这样可以防止大模型（edge 极大）或小面片（denom 极小）主导切线方向并产生 Inf/NaN。
				if (glm::length(faceTangent) > 0.0001f) faceTangent = glm::normalize(faceTangent);
				if (glm::length(faceBitangent) > 0.0001f) faceBitangent = glm::normalize(faceBitangent);

				v0.tangent += faceTangent; v1.tangent += faceTangent; v2.tangent += faceTangent;
				v0.bitangent += faceBitangent; v1.bitangent += faceBitangent; v2.bitangent += faceBitangent;
			}

			// 3. 正交化 (Gram-Schmidt)
			for (auto& v : vertices) {
				glm::vec3 t = v.tangent - v.normal * glm::dot(v.normal, v.tangent);

				if (glm::length(t) < 0.0001f) {
					// 伪造合法的切线
					glm::vec3 up = std::abs(v.normal.y) < 0.999f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
					v.tangent = glm::normalize(glm::cross(up, v.normal));
				}
				else {
					v.tangent = glm::normalize(t);
				}

				// 构建完美的正交 TBN 矩阵
				// 不要直接 Normalize 累加得到的副切线，它不一定和法线/切线完全垂直。
				// 通过叉乘生成完美正交的副切线，再利用累加的副切线判断 UV 镜像方向（手性）。
				float handedness = glm::dot(glm::cross(v.normal, v.tangent), v.bitangent) < 0.0f ? -1.0f : 1.0f;
				v.bitangent = handedness * glm::normalize(glm::cross(v.normal, v.tangent));
			}
		}

		// 简单的绘制立方体函数
		static void RenderCube()
		{
			if (s_CubeVAO == nullptr) {
				// ------------- 构建简单的 Cube VAO ----------------
				std::vector<Vertex> vertices = {
					// 1. 前面 (Front Face) - Z = 1.0f
					{ {-1.0f, -1.0f,  1.0f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f} },
					{ { 1.0f, -1.0f,  1.0f},  {0.0f, 0.0f, 1.0f},  {1.0f, 0.0f} },
					{ { 1.0f,  1.0f,  1.0f},  {0.0f, 0.0f, 1.0f},  {1.0f, 1.0f} },
					{ {-1.0f,  1.0f,  1.0f},  {0.0f, 0.0f, 1.0f},  {0.0f, 1.0f} },
					// 2. 右面 (Right Face) - X = 1.0f
					{ { 1.0f, -1.0f,  1.0f},  {1.0f, 0.0f, 0.0f},  {0.0f, 0.0f} },
					{ { 1.0f, -1.0f, -1.0f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f} },
					{ { 1.0f,  1.0f, -1.0f},  {1.0f, 0.0f, 0.0f},  {1.0f, 1.0f} },
					{ { 1.0f,  1.0f,  1.0f},  {1.0f, 0.0f, 0.0f},  {0.0f, 1.0f} },
					// 3. 后面 (Back Face) - Z = -1.0f
					{ { 1.0f, -1.0f, -1.0f},  {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
					{ {-1.0f, -1.0f, -1.0f},  {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
					{ {-1.0f,  1.0f, -1.0f},  {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },
					{ { 1.0f,  1.0f, -1.0f},  {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },
					// 4. 左面 (Left Face) - X = -1.0f
					{ {-1.0f, -1.0f, -1.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
					{ {-1.0f, -1.0f,  1.0f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
					{ {-1.0f,  1.0f,  1.0f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
					{ {-1.0f,  1.0f, -1.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
					// 5. 上面 (Top Face) - Y = 1.0f
					{ {-1.0f,  1.0f,  1.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 0.0f} },
					{ { 1.0f,  1.0f,  1.0f},  {0.0f, 1.0f, 0.0f},  {1.0f, 0.0f} },
					{ { 1.0f,  1.0f, -1.0f},  {0.0f, 1.0f, 0.0f},  {1.0f, 1.0f} },
					{ {-1.0f,  1.0f, -1.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f} },
					// 6. 下面 (Bottom Face) - Y = -1.0f
					{ {-1.0f, -1.0f, -1.0f},  {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
					{ { 1.0f, -1.0f, -1.0f},  {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
					{ { 1.0f, -1.0f,  1.0f},  {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
					{ {-1.0f, -1.0f,  1.0f},  {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} }
				};
				std::vector<uint32_t> indices = {
					0, 1, 2, 2, 3, 0,       // 前面
					4, 5, 6, 6, 7, 4,       // 右面
					8, 9, 10, 10, 11, 8,    // 后面
					12, 13, 14, 14, 15, 12, // 左面
					16, 17, 18, 18, 19, 16, // 上面
					20, 21, 22, 22, 23, 20  // 下面
				};
				s_CubeVAO = VertexArray::Create();
				s_CubeVAO->Bind();
				uint32_t verticesSize = static_cast<uint32_t>(vertices.size() * sizeof(Vertex));
				Refptr<VertexBuffer> vbo = VertexBuffer::Create(vertices.data(), verticesSize);
				vbo->Bind();
				Refptr<BufferLayout> layout = BufferLayout::Create({
						{ "a_Position",  VertexDataType::Float3 },
						{ "a_Normal",    VertexDataType::Float3 },
						{ "a_TexCoord",  VertexDataType::Float2 },
						{ "a_Tangent",  VertexDataType::Float3 },
						{ "a_Bitangent",  VertexDataType::Float3 }
					});
				vbo->SetLayout(layout);
				s_CubeVAO->SetVertexBuffer(vbo);
				uint32_t indicesSize = static_cast<uint32_t>(indices.size() * sizeof(uint32_t));
				Refptr<IndexBuffer> ibo = IndexBuffer::Create(indices.data(), indicesSize);
				s_CubeVAO->SetIndexBuffer(ibo);
			}
			s_CubeVAO->Bind();
			RendererCommand::Clear();
			RendererCommand::DrawIndexed(s_CubeVAO);
			s_CubeVAO->Unbind();
		}

		static Refptr<Texture> LoadEquirectangularToCubemap(const std::string& path)
		{
			// --------- 路径检查 (仅支持 .exr 和 .hdr) -------------
			std::filesystem::path srcPath(path);
			if (!std::filesystem::exists(srcPath)) return nullptr;

			std::string ext = srcPath.extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

			bool isEXR = (ext == ".exr");
			bool isHDR = (ext == ".hdr");

			if (!isEXR && !isHDR) {
				SNL_CORE_WARN("TextureLibrary::LoadEquirectangularToCubemap: 仅支持 .HDR & .EXR 格式的等距柱状图，目前格式：{0}", ext);
				return nullptr;
			}

			std::string stem = srcPath.stem().string();
			std::string parentDir = srcPath.parent_path().string();
			if (!parentDir.empty()) parentDir += "/";

			// -------------- 加载原始纹理资源 --------------
			auto texture2D = Texture2D::Create({ path }, TextureUsage::None);
			if (!texture2D || texture2D->GetRendererId() == 0) return nullptr;

			// ------------- 创建 FBO --------------
			int dim = 2048;
			// 使用 RGBA32F 浮点格式
			FrameBufferSpecification spec(dim, dim);
			spec.attachments = { FrameBufferTextureFormat::RGBA32F };
			auto captureFBO = FrameBuffer::Create(spec);

			// ------------- 创建 shader & 绑定一部分uniforms --------------
			auto shader = ShaderLibrary::Load("Temp_Shader_Equirectangular_To_Cubemap", "assets/shaders/EquirectangularToCubemap.glsl", {});
			shader->Bind();
			shader->SetInt("u_EnvironmentMap", 0);
			shader->SetMat4("u_Projection", captureProjection);

			// -------- 创建一个空的立方体贴图（没有数据，但分配内存）---------
			auto textureCube = TextureCube::Create(dim);

			// -------------- 渲染到帧缓冲区（绘制到贴图的6个面） -------------
			texture2D->Bind(0);
			captureFBO->Bind();
			for (unsigned int i = 0; i < 6; ++i)
			{
				shader->SetMat4("u_View", captureViews[i]);
				captureFBO->SetupTextureCubei(i, textureCube->GetRendererId()); // 关联到 Cubemap

				RenderCube();
			}

			captureFBO->Unbind();
			textureCube->Unbind();

			// 设置成单个路径 且 
			// usage 默认是 texture_cubemap
			// 这两个就是特殊处理的条件
			textureCube->SetPath({ path });
			return textureCube;
		}

		static Refptr<Texture> CalculateCubemapToIBLIrradianceConvolution(const Refptr<Texture>& cubemap)
		{
			int dim = 32;
			auto irradianceMap = TextureCube::Create(dim);

			FrameBufferSpecification spec(dim, dim);
			spec.attachments = { FrameBufferTextureFormat::RGBA32F };
			auto captureFBO = FrameBuffer::Create(spec);

			auto irradianceShader = ShaderLibrary::Load("IBL_IrradianceShader_" + TexturePathsToKey(cubemap->GetPath(), cubemap->GetUsage()), "assets/shaders/IBL_IrradianceConvolution.glsl", {});
			irradianceShader->Bind();
			irradianceShader->SetInt("u_EnvironmentMap", 0);
			irradianceShader->SetMat4("u_Projection", captureProjection);
			cubemap->Bind(0);

			// 渲染循环
			captureFBO->Bind();
			for (unsigned int i = 0; i < 6; ++i)
			{
				irradianceShader->SetMat4("u_View", captureViews[i]);

				captureFBO->SetupTextureCubei(i, irradianceMap->GetRendererId()); // 关联到 Cubemap

				RenderCube();
			}
			captureFBO->Unbind();

			return irradianceMap;
		}

		static Refptr<Texture> CalculateCubemapToIBLPrefilter(const Refptr<Texture>& cubemap, int prefilterMapResolution = 128, int maxPrefilterMipLevels = 5)
		{
			bool genMipmap = true;
			auto prefilterMap = TextureCube::Create(prefilterMapResolution, genMipmap);

			FrameBufferSpecification spec(prefilterMapResolution, prefilterMapResolution);
			spec.attachments = { FrameBufferTextureFormat::RGBA32F };
			auto captureFBO = FrameBuffer::Create(spec);

			auto prefilterShader = ShaderLibrary::Load("IBL_PrefilterShader", "assets/shaders/IBL_Prefilter.glsl", {});
			prefilterShader->Bind();
			prefilterShader->SetInt("u_EnvironmentMap", 0);
			prefilterShader->SetMat4("u_Projection", captureProjection);
			prefilterShader->SetFloat("u_Resolution", (float)prefilterMapResolution);
			cubemap->Bind(0);

			captureFBO->Bind();
			for (int mip = 0; mip < maxPrefilterMipLevels; ++mip)
			{
				// 根据 mip 级别调整 FBO 大小
				unsigned int mipWidth = prefilterMapResolution * std::pow(0.5, mip);
				unsigned int mipHeight = prefilterMapResolution * std::pow(0.5, mip);
				captureFBO->Resize(mipWidth, mipHeight);

				float roughness = (float)mip / (float)(maxPrefilterMipLevels - 1);
				prefilterShader->SetFloat("u_Roughness", roughness);

				captureFBO->Bind();
				// 渲染循环
				for (unsigned int i = 0; i < 6; ++i)
				{
					prefilterShader->SetMat4("u_View", captureViews[i]);

					captureFBO->SetupTextureCubei(i, prefilterMap->GetRendererId(), mip); // 关联到 Cubemap

					RenderCube();
				}
			}
			captureFBO->Unbind();

			return prefilterMap;
		}

		static std::string TexturePathsToKey(const std::vector<std::string>& paths, const TextureUsage& usage) {
			if (paths.size() == 1) return paths[0] + "|" + TextureUsageToString(usage) + ";";

			std::string key;
			for (const auto& p : paths) {
				key += p + "|" + TextureUsageToString(usage) + ";";
			}

			return key;
		}
	};

}
