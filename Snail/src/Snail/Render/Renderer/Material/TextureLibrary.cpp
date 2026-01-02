#include "SNLpch.h"

#include "ShaderLibrary.h"
#include "TextureLibrary.h"

#include "Snail/Render/RenderAPI/VertexArray.h"
#include "Snail/Render/Renderer/FrameBuffer/FrameBuffer.h"
#include "Snail/Render/Renderer/RendererCommand.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Snail {

	std::unordered_map<std::string, Refptr<Texture>> TextureLibrary::m_TextureNames = std::unordered_map<std::string, Refptr<Texture>>();
	std::unordered_map<std::string, Refptr<Texture>> TextureLibrary::m_TexturePaths = std::unordered_map<std::string, Refptr<Texture>>();

	Refptr<Texture> TextureLibrary::Load(const std::vector<std::string>& filePath, const TextureUsage& usage)
	{
		// 从路径提取名字 (例如 assets/images/color.png -> color)
		std::filesystem::path path = filePath[0];		// 默认首个
		std::string name = path.u8string() + "_" + TextureUsageToString(usage);

		std::string pathKey = PathsToKey(filePath, usage);
		
		if (m_TexturePaths.find(pathKey) != m_TexturePaths.end()) { // 资源已经加载过
			if (m_TextureNames.find(name) == m_TextureNames.end()) { // 但是找不到对应命名
				m_TextureNames[name] = m_TexturePaths[pathKey]; // 把 pathKey 对应的纹理对象添加到命名映射表
			} // else 否则就是完全相同，不做操作
			return m_TexturePaths[pathKey];
		}

		if (m_TextureNames.find(name) != m_TextureNames.end()) { // 命名已经存在
			SNL_CORE_WARN("TextureLibrary: 名字 '{0}' 已被占用，但指向的是不同资源！", name); // 新加载一个纹理
			//return m_TextureNames[customName];
		}

		Refptr<Texture> texture;
		if (filePath.size() == 1 && usage != TextureUsage::Cubemap) {
			texture = Texture2D::Create(filePath, usage);	
		}
		else if (filePath.size() == 1 && usage == TextureUsage::Cubemap) {
			texture = LoadEquirectangularToCubemap(filePath[0]);
		}
		else if (filePath.size() > 1 && usage == TextureUsage::Cubemap) {
			texture = TextureCube::Create(filePath, usage);
		}
		m_TextureNames[name] = texture;
		m_TexturePaths[pathKey] = texture;

		return texture;
	}

	Refptr<Texture> TextureLibrary::Load(const std::string& customName, const std::vector<std::string>& filePath, const TextureUsage& usage)
	{
		std::string pathKey = PathsToKey(filePath, usage);

		if (m_TexturePaths.find(pathKey) != m_TexturePaths.end()) { // 资源已经加载过
			if (m_TextureNames.find(customName) == m_TextureNames.end()) { // 但是找不到对应命名
				m_TextureNames[customName] = m_TexturePaths[pathKey]; // 把 pathKey 对应的纹理对象添加到命名映射表
			} // else 否则就是完全相同，不做操作
			return m_TexturePaths[pathKey];
		}

		if (m_TextureNames.find(customName) != m_TextureNames.end()) { // 命名已经存在
			SNL_CORE_WARN("TextureLibrary: 名字 '{0}' 已被占用，但指向的是不同资源！", customName); // 新加载一个纹理
			//return m_TextureNames[customName];
		}

		Refptr<Texture> texture;
		if (filePath.size() == 1 && usage != TextureUsage::Cubemap) {
			texture = Texture2D::Create(filePath, usage);
		}
		else if (filePath.size() == 1 && usage == TextureUsage::Cubemap) {
			texture = LoadEquirectangularToCubemap(filePath[0]);
		}
		else if (filePath.size() > 1 && usage == TextureUsage::Cubemap) {
			texture = TextureCube::Create(filePath, usage);
		}
		m_TextureNames[customName] = texture;
		m_TexturePaths[pathKey] = texture;

		return texture;
	}

	Refptr<Texture> TextureLibrary::Get(const std::string& name)
	{
		int status = m_TextureNames.find(name) != m_TextureNames.end(); // 1表示name在映射表中已经存在
		SNL_CORE_ASSERT(status, "TextureLibrary: 纹理库不存在该 texture '{0}' ", name);

		return m_TextureNames[name];
	}

	void TextureLibrary::Remove(const std::string& name)
	{
		if (m_TextureNames.find(name) != m_TextureNames.end()) {
			m_TextureNames.erase(name);
		}
	}

	void TextureLibrary::Remove(const Refptr<Texture>& texture)
	{
		std::string path = PathsToKey(texture->GetPath(), texture->GetUsage());
		if (m_TexturePaths.find(path) != m_TexturePaths.end()) {
			m_TexturePaths.erase(path);
		}
	}

	std::string TextureLibrary::PathsToKey(const std::vector<std::string>& paths, const TextureUsage& usage) {
		if (paths.size() == 1) return paths[0] + "|" + TextureUsageToString(usage) + ";";

		std::string key;
		for (const auto& p : paths) {
			key += p + "|" + TextureUsageToString(usage) + ";";
		}

		return key;
	}

	Refptr<Texture> TextureLibrary::LoadEquirectangularToCubemap(const std::string& path)
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
		spec.colorFormat = FrameBufferColorFormat::RGBA32F;
		auto captureFBO = FrameBuffer::Create(spec);

		// ------------- 一些临时的相机视角 --------------
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] = {
			glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // +X (Right) - Up is Down
			glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // -X (Left)  - Up is Down
			glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)), // +Y (Top)   - Up is +Z
			glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)), // -Y (Bottom)- Up is -Z
			glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // +Z (Front) - Up is Down
			glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))  // -Z (Back)  - Up is Down
		};

		// ------------- 创建 shader & 绑定一部分uniforms --------------
		auto shader = ShaderLibrary::Load("Temp_Shader_Equirectangular_To_Cubemap", "assets/shaders/EquirectangularToCubemap.glsl", {});
		shader->Bind();
		shader->SetInt("equirectangularMap", 0);
		shader->SetMat4("projection", captureProjection);

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
		auto cubeVAO = VertexArray::Create();
		cubeVAO->Bind();
		uint32_t verticesSize = static_cast<uint32_t>(vertices.size() * sizeof(Vertex));
		Refptr<VertexBuffer> vbo = VertexBuffer::Create(vertices.data(), verticesSize);
		vbo->Bind();
		Refptr<BufferLayout> layout = BufferLayout::Create({
				{ "a_Position",  VertexDataType::Float3 },
				{ "a_Normal",    VertexDataType::Float3 },
				{ "a_TexCoord",  VertexDataType::Float2 }
			});
		vbo->SetLayout(layout);
		cubeVAO->SetVertexBuffer(vbo);
		uint32_t indicesSize = static_cast<uint32_t>(indices.size() * sizeof(uint32_t));
		Refptr<IndexBuffer> ibo = IndexBuffer::Create(indices.data(), indicesSize);
		cubeVAO->SetIndexBuffer(ibo);

		// -------- 创建一个空的立方体贴图（没有数据，但分配内存）---------
		auto textureCube = TextureCube::Create(dim);

		// -------------- 渲染到帧缓冲区（绘制到贴图的6个面） -------------
		texture2D->Bind(0);
		captureFBO->Bind();
		cubeVAO->Bind();
		for (unsigned int i = 0; i < 6; ++i)
		{
			shader->SetMat4("view", captureViews[i]);
			captureFBO->SetupTextureCubei(i, textureCube->GetRendererId()); // 关联到 Cubemap

			RendererCommand::Clear();
			RendererCommand::DrawIndexed(cubeVAO);
		}

		captureFBO->Unbind();
		textureCube->Unbind();

		// 设置成单个路径 且 
		// usage 默认是 texture_cubemap
		// 这两个就是特殊处理的条件
		textureCube->SetPath({ path });
		return textureCube;
	}

}