#include "SNLpch.h"

#include "Snail/Render/Renderer/Material/ShaderLibrary.h"
#include "Snail/Render/Renderer/Renderer3D.h"
#include "Snail/Utils/RendererTools.h"

#include "Model.h"

namespace Snail {

	Model::Model(const Refptr<Mesh>& mesh)
		: m_IsImported(false)
	{
		if (mesh->GetVAO() && mesh->GetMaterial()) {

			m_DefaultShaderPath = mesh->GetMaterial()->GetShader()->GetFilePath();

			m_AABB.min = glm::vec3(std::numeric_limits<float>::max());
			m_AABB.max = glm::vec3(std::numeric_limits<float>::lowest());
			ConsiderMeshAABB(mesh);

			m_Meshes.push_back(mesh);
		}
	}

	Model::Model(const PrimitiveType& type, const Refptr<Shader>& shader, const std::vector<Refptr<Texture>>& textures, const glm::mat4& localTransform)
		: m_DefaultShaderPath(shader->GetFilePath()), m_IsImported(false), m_PrimitiveType(type)
	{
		auto [vertices, indices] = GetPrimitiveDatas(type);

		Refptr<Mesh> mesh = std::make_shared<Mesh>(type, vertices, indices, shader, textures, localTransform);

		m_AABB.min = glm::vec3(std::numeric_limits<float>::max());
		m_AABB.max = glm::vec3(std::numeric_limits<float>::lowest());
		ConsiderMeshAABB(mesh);

		m_Meshes.push_back(mesh);
	}

	Model::Model(const PrimitiveType& type, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Refptr<Shader>& shader, const std::vector<Refptr<Texture>>& textures, const glm::mat4& localTransform)
		: m_DefaultShaderPath(shader->GetFilePath()), m_IsImported(false), m_PrimitiveType(type)
	{
		Refptr<Mesh> mesh = std::make_shared<Mesh>(type, vertices, indices, shader, textures, localTransform);

		m_AABB.min = glm::vec3(std::numeric_limits<float>::max());
		m_AABB.max = glm::vec3(std::numeric_limits<float>::lowest());
		ConsiderMeshAABB(mesh);

		m_Meshes.push_back(mesh);
	}

	Model::Model(const Refptr<Shader>& shader, const std::string& objPath)
		: m_DefaultShaderPath(shader->GetFilePath()), m_IsImported(true)
	{
		Load(objPath);
	}

	void Model::Draw(const glm::mat4& worldTransform, const bool& edgeEnable) const
	{
		SNL_PROFILE_FUNCTION();


		Renderer3D::DrawModel(*this, edgeEnable, worldTransform);
	}

	void Model::Load(const std::string& path)
	{
		SNL_PROFILE_FUNCTION();


		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(
			path,
			//aiProcess_CalcTangentSpace |	// 计算切线和副切线（debug模式很慢）
			//aiProcess_FlipUVs |				// 翻转y轴
			aiProcess_Triangulate |			// 分割成三角形
			aiProcess_GenNormals			// 生成法线
		);

		// 读取数据是否非空 & 读取数据是否完整
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			SNL_CORE_ERROR("Model Importer 读取模型错误: ERROR::ASSIMP:: '{0}'", importer.GetErrorString());
			SNL_CORE_ASSERT(false, "Model Importer 读取模型错误");
			return;
		}
		// 示例：assets/models/sponza/sponza.obj
		std::string tempPath = RendererTools::CleanFilePath(std::string(path));
		tempPath = RendererTools::CleanWindowsPath(tempPath);
		std::filesystem::path stdPath = tempPath;
		m_FullPath = tempPath;

		// parent_path() 自动提取目录
	    m_Directory = stdPath.parent_path().u8string(); 

		ProcessNode(scene->mRootNode, scene, glm::mat4(1.0f));

		// 处理AABB
		m_AABB.min = glm::vec3(std::numeric_limits<float>::max());
		m_AABB.max = glm::vec3(std::numeric_limits<float>::lowest());
		for (const auto& mesh : m_Meshes) {
			ConsiderMeshAABB(mesh);
		}
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransformation)
	{
		glm::mat4 nodeTransformation = ConvertaiMat4ToglmMat4(node->mTransformation);

		// 处理节点所有的网格（如果有的话）
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			m_Meshes.push_back(ProcessMesh(mesh, scene, parentTransformation * nodeTransformation));
		}
		// 接下来对它的子节点重复这一过程
		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene, parentTransformation * nodeTransformation);
		}
	}

	Refptr<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& localTransformation)
	{
		SNL_PROFILE_FUNCTION();


		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Refptr<Texture>> textures;

		// 处理顶点位置、法线和纹理坐标
		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			// --- 读取顶点 ---
			vertex.position.x = static_cast<float>(mesh->mVertices[i].x);
			vertex.position.y = static_cast<float>(mesh->mVertices[i].y);
			vertex.position.z = static_cast<float>(mesh->mVertices[i].z);
			// --- 读取法线 ---
			if (mesh->HasNormals())
			{
				vertex.normal.x = static_cast<float>(mesh->mNormals[i].x);
				vertex.normal.y = static_cast<float>(mesh->mNormals[i].y);
				vertex.normal.z = static_cast<float>(mesh->mNormals[i].z);
			}
			else
			{
				// 如果没有法线，给一个默认值（比如朝上），防止数据未初始化
				vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			// --- 读取UV ---
			if (mesh->HasTextureCoords(0))
			{
				vertex.texCoords.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
				vertex.texCoords.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
			}
			else
			{
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}
			// --- 读取切线和副切线 ---
			//if (mesh->HasTangentsAndBitangents())
			//{
			//	vertex.tangent.x = static_cast<float>(mesh->mTangents[i].x);
			//	vertex.tangent.y = static_cast<float>(mesh->mTangents[i].y);
			//	vertex.tangent.z = static_cast<float>(mesh->mTangents[i].z);

			//	vertex.bitangent.x = static_cast<float>(mesh->mBitangents[i].x);
			//	vertex.bitangent.y = static_cast<float>(mesh->mBitangents[i].y);
			//	vertex.bitangent.z = static_cast<float>(mesh->mBitangents[i].z);
			//}
			//else
			//{
			//	// 默认值，防止由未初始化导致的渲染错误
			//	vertex.tangent = glm::vec3(0.0f);
			//	vertex.bitangent = glm::vec3(0.0f);
			//}
			
			vertices.push_back(vertex);
		}

		// 处理索引
		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// --- 手动计算切线和副切线 ---
		RendererTools::RecalculateTangents(vertices, indices);

		// 处理材质（纹理）
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			std::vector<Refptr<Texture>> diffuseMaps = LoadMaterialTextures(scene, material, aiTextureType_DIFFUSE, TextureUsage::Diffuse);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<Refptr<Texture>> specularMaps = LoadMaterialTextures(scene, material, aiTextureType_SPECULAR, TextureUsage::Specular);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			std::vector<Refptr<Texture>> albedoMaps = LoadMaterialTextures(scene, material, aiTextureType_BASE_COLOR, TextureUsage::Albedo);
			if (albedoMaps.empty()) {
				albedoMaps = LoadMaterialTextures(scene, material, aiTextureType_DIFFUSE, TextureUsage::Albedo);
			}
			textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());

			std::vector<Refptr<Texture>> metallicMaps = LoadMaterialTextures(scene, material, aiTextureType_METALNESS, TextureUsage::Metallic);
			textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());

			std::vector<Refptr<Texture>> roughnessMaps = LoadMaterialTextures(scene, material, aiTextureType_DIFFUSE_ROUGHNESS, TextureUsage::Roughness);
			if (roughnessMaps.empty()) {
				// 某些旧格式可能会用 Shininess 来模拟
				roughnessMaps = LoadMaterialTextures(scene, material, aiTextureType_SHININESS, TextureUsage::Roughness);
			}
			textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

			// 很多 glTF 模型会将 AO 放在 aiTextureType_LIGHTMAP 中，或者 aiTextureType_AMBIENT_OCCLUSION
			std::vector<Refptr<Texture>> aoMaps = LoadMaterialTextures(scene, material, aiTextureType_AMBIENT_OCCLUSION, TextureUsage::AO);
			if (aoMaps.empty()) {
				aoMaps = LoadMaterialTextures(scene, material, aiTextureType_LIGHTMAP, TextureUsage::AO);
			}
			textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());

			// 用于处理发光的材质（如霓虹灯、屏幕等）
			std::vector<Refptr<Texture>> emissiveMaps = LoadMaterialTextures(scene, material, aiTextureType_EMISSIVE, TextureUsage::Emissive);
			textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

			std::vector<Refptr<Texture>> normalMaps = LoadMaterialTextures(scene, material, aiTextureType_NORMALS, TextureUsage::Normal);
			if (normalMaps.empty()) {
				// 很多 FBX 把法线贴图放在 Height 通道
				normalMaps = LoadMaterialTextures(scene, material, aiTextureType_HEIGHT, TextureUsage::Normal);
			}
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			// TODO: cube 纹理
			//std::vector<Refptr<Texture>> cubeMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, TextureUsage::Cubemap);
			//textures.insert(textures.end(), cubeMaps.begin(), cubeMaps.end());
		}

		auto defaultShader = ShaderLibrary::Load(m_DefaultShaderPath, {});
		Refptr<Mesh> resultMesh = std::make_shared<Mesh>(PrimitiveType::None, vertices, indices, defaultShader, textures, localTransformation);

		// 处理材质（颜色）
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			// 1. 读取漫反射颜色 (Diffuse Color / Kd)
			aiColor3D diffuseColor(0.8f, 0.8f, 0.8f); // 默认灰色
			if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
			{
				// 将读取到的颜色传给材质
				resultMesh->GetMaterial()->SetFloat3("u_ColorDiffuse", glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b));
			}

			// 2. 读取镜面光颜色 (Specular Color / Ks)
			aiColor3D specularColor(0.5f, 0.5f, 0.5f);
			if (material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == AI_SUCCESS)
			{
				resultMesh->GetMaterial()->SetFloat3("u_ColorSpecular", glm::vec3(specularColor.r, specularColor.g, specularColor.b));
			}

			// 3. 读取环境光颜色 (Ambient Color / Ka)
			aiColor3D ambientColor(0.1f, 0.1f, 0.1f);
			if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == AI_SUCCESS)
			{
				resultMesh->GetMaterial()->SetFloat3("u_ColorAmbient", glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b));
			}

			// 4. 读取光泽度 (Shininess / Ns)
			float shininess = 32.0f;
			if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
			{
				resultMesh->GetMaterial()->SetFloat("u_Shininess", shininess);
			}
		}

		return resultMesh;
	}

	std::vector<Refptr<Texture>> Model::LoadMaterialTextures(const aiScene* scene, aiMaterial* mat, const aiTextureType& type, const TextureUsage& usage)
	{
		SNL_PROFILE_FUNCTION();


		std::vector<Refptr<Texture>> textures;

		for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			// 检查 Assimp 是否成功获取了路径
			if (mat->GetTexture(type, i, &str) != AI_SUCCESS) {
				SNL_CORE_WARN("Model: Assimp Failed to get texture path for type {0}, index {1}", TextureUsageToString(usage), i);
				continue;
			}

			// --------------------------- 嵌入式纹理 ----------------------------------
			// 路径以 '*' 开头，表示它在 aiScene::mTextures 数组中
			if (str.C_Str()[0] == '*')
			{
				if (scene->HasTextures())
				{
					int textureIndex = atoi(str.C_Str() + 1); // 解析 "*0" -> 0

					if (textureIndex < scene->mNumTextures)
					{
						aiTexture* aiTex = scene->mTextures[textureIndex];

						Refptr<Texture> texture;

						// mHeight == 0 表示它是压缩格式 (如 png/jpg 文件的二进制流)
						if (aiTex->mHeight == 0)
						{
							SNL_CORE_TRACE("Model: 加载嵌入的压缩纹理 (Index: {0}, Size: {1} bytes)", textureIndex, aiTex->mWidth);

							// aiTex->pcData 指向数据首地址
							// aiTex->mWidth 是数据的大小（字节数）
							// aiTex->achFormatHint 包含格式提示 (如 "png", "jpg")
							std::string pathKey = "Embedded_*" + std::to_string(textureIndex) + "_" + TextureUsageToString(usage);
							texture = TextureLibrary::Load(aiTex->pcData, aiTex->mWidth, usage, pathKey);
						}
						else
						{
							// mHeight > 0 表示它是原始的 ARGB8888 像素数据
							SNL_CORE_TRACE("Model: 加载嵌入的原始纹理 (Index: {0}, W: {1}, H: {2})", textureIndex, aiTex->mWidth, aiTex->mHeight);

							// 数据大小通常是 mWidth * mHeight * 4 (RGBA)
							// texture = TextureLibrary::LoadRawData(aiTex->pcData, aiTex->mWidth, aiTex->mHeight, usage);
						}

						if (texture) textures.push_back(texture);
					}
				}
				continue; // 处理完嵌入纹理后跳过本次循环，不走下面的文件加载逻辑
			}

			// ------------------------ 从文件系统加载 --------------------------
			// 防御空路径
			if (str.length == 0) {
				SNL_CORE_WARN("Model: Texture path is empty!");
				continue;
			}
			std::string filename = RendererTools::CleanWindowsPath(str.C_Str());
			//filename = RendererTools::CleanFilePath(filename);

			Refptr<Texture> texture;

			std::filesystem::path filePath(filename);
			if (filePath.extension() == "") {
				filename += ".png";
			}

			std::string fullPath = m_Directory + '/' + filename;

			SNL_CORE_TRACE("Model: 加载纹理路径: '{0}' 中...", fullPath);

			// TextureLibrary有缓存，这里直接Load，在内部会判断缓存
			texture = TextureLibrary::Load({ fullPath }, usage);
			if (texture)
				textures.push_back(texture);
			else
				SNL_CORE_ERROR("Model: 纹理加载失败或路径无效: {0}", fullPath);
		}
		return textures;
	}

	//--------------Tools--------------------
	glm::mat4 Model::ConvertaiMat4ToglmMat4(const aiMatrix4x4& matrix) const
	{
		glm::mat4 to;
		// Assimp: Row-Major (a1, a2, a3, a4 是第一行)
		// GLM: Column-Major
		// 手动把 Assimp 的行，赋值给 GLM 的行 (GLM[col][row])
		to[0][0] = static_cast<float>(matrix.a1); to[1][0] = static_cast<float>(matrix.a2); to[2][0] = static_cast<float>(matrix.a3); to[3][0] = static_cast<float>(matrix.a4);
		to[0][1] = static_cast<float>(matrix.b1); to[1][1] = static_cast<float>(matrix.b2); to[2][1] = static_cast<float>(matrix.b3); to[3][1] = static_cast<float>(matrix.b4);
		to[0][2] = static_cast<float>(matrix.c1); to[1][2] = static_cast<float>(matrix.c2); to[2][2] = static_cast<float>(matrix.c3); to[3][2] = static_cast<float>(matrix.c4);
		to[0][3] = static_cast<float>(matrix.d1); to[1][3] = static_cast<float>(matrix.d2); to[2][3] = static_cast<float>(matrix.d3); to[3][3] = static_cast<float>(matrix.d4);

		return to;
	}

	void Model::ConsiderMeshAABB(const Refptr<Mesh>& mesh)
	{
		glm::vec3 min = mesh->m_MinVertex;
		glm::vec3 max = mesh->m_MaxVertex;
		glm::mat4 transform = mesh->GetLocationTransform();
		glm::vec3 corners[8] = {
			{min.x, min.y, min.z}, {min.x, min.y, max.z},
			{min.x, max.y, min.z}, {min.x, max.y, max.z},
			{max.x, min.y, min.z}, {max.x, min.y, max.z},
			{max.x, max.y, min.z}, {max.x, max.y, max.z}
		};
		for (int i = 0; i < 8; i++) {
			glm::vec3 p = glm::vec3(transform * glm::vec4(corners[i], 1.0f));
			m_AABB.min = glm::min(m_AABB.min, p);
			m_AABB.max = glm::max(m_AABB.max, p);
		}
	}

	std::pair<std::vector<Vertex>, std::vector<uint32_t>> Model::GetPrimitiveDatas(const PrimitiveType& type)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		const float PI = 3.14159265359f;

		switch (type)
		{
		case PrimitiveType::Cube:
			// 立方体：切线方向与 UV 的 U 方向一致
			vertices = {
				// 格式: {Pos}, {Normal}, {UV}, {Tangent}, {Bitangent}

				// 1. 前面 (Front) - Z+ | Tangent 指向 X+
				{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
				{ { 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
				{ { 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
				{ {-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },

				// 2. 右面 (Right) - X+ | Tangent 指向 Z- (因为贴图是绕着贴的)
				{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f, 0.0f},  {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },
				{ { 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },
				{ { 0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f},  {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },
				{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 0.0f},  {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f} },

				// 3. 后面 (Back) - Z- | Tangent 指向 X-
				{ { 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
				{ {-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
				{ {-0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
				{ { 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },

				// 4. 左面 (Left) - X- | Tangent 指向 Z+
				{ {-0.5f, -0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f} },
				{ {-0.5f, -0.5f,  0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f} },
				{ {-0.5f,  0.5f,  0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f} },
				{ {-0.5f,  0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f} },

				// 5. 上面 (Top) - Y+ | Tangent 指向 X+
				{ {-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f},  {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
				{ { 0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f},  {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
				{ { 0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f},  {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
				{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },

				// 6. 下面 (Bottom) - Y- | Tangent 指向 X+
				{ {-0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} },
				{ { 0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} },
				{ { 0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} },
				{ {-0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }
			};

			indices = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20 };
			break;

		case PrimitiveType::Sphere:
		{
			const int X_SEGMENTS = 64;
			const int Y_SEGMENTS = 64;
			const float radius = 0.5f;

			for (int y = 0; y <= Y_SEGMENTS; ++y)
			{
				for (int x = 0; x <= X_SEGMENTS; ++x)
				{
					float xSegment = (float)x / (float)X_SEGMENTS;
					float ySegment = (float)y / (float)Y_SEGMENTS;

					float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
					float yPos = std::cos(ySegment * PI);
					float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

					glm::vec3 normal = glm::vec3(xPos, yPos, zPos);
					glm::vec3 pos = normal * radius;
					glm::vec2 uv = glm::vec2(xSegment, ySegment);

					// --- 动态计算球面切线 ---
					// 切线 T 是沿着纬度线（Longitude）方向的导数
					// 简单的几何推导：垂直于 Normal，且在 XZ 平面上旋转
					// T = (-sin(theta), 0, cos(theta)) 对应 xPos/zPos 的生成逻辑
					glm::vec3 tangent = glm::normalize(glm::vec3(-std::sin(xSegment * 2.0f * PI), 0.0f, std::cos(xSegment * 2.0f * PI)));

					// 副切线 B 直接叉乘得到 (遵循右手定则)
					glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

					vertices.push_back({ pos, normal, uv, tangent, bitangent });
				}
			}
			// 索引生成逻辑
			bool oddRow = false;
			for (int y = 0; y < Y_SEGMENTS; ++y)
			{
				for (int x = 0; x < X_SEGMENTS; ++x)
				{
					uint32_t k1 = y * (X_SEGMENTS + 1) + x;
					uint32_t k2 = k1 + X_SEGMENTS + 1;
					indices.push_back(k1); indices.push_back(k2); indices.push_back(k1 + 1);
					indices.push_back(k1 + 1); indices.push_back(k2); indices.push_back(k2 + 1);
				}
			}
			break;
		}

		case PrimitiveType::Plane:
		{
			float size = 0.5f;
			// 平面：XZ 平面，法线向上(0,1,0)，切线自然是 X 轴(1,0,0)，副切线是 Z 轴(0,0,1)或(0,0,-1)
			vertices = {
				// Pos                  // Normal          // UV         // Tangent         // Bitangent
				{ {-size, 0.0f, -size}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
				{ {-size, 0.0f,  size}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
				{ { size, 0.0f,  size}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
				{ { size, 0.0f, -size}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }
			};
			indices = { 0, 1, 2, 2, 3, 0 };
			break;
		}

		case PrimitiveType::Skybox: // 天空盒
			vertices = {
				// Positions          // Normals           // UVs (Unused)
				{ {-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 0. 左下前
				{ { 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 1. 右下前
				{ { 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 2. 右上前
				{ {-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 3. 左上前
				{ {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 4. 左下后
				{ { 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 5. 右下后
				{ { 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 6. 右上后
				{ {-1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }  // 7. 左上后
			};
			indices = {
				0, 1, 2, 2, 3, 0,
				1, 5, 6, 6, 2, 1,
				5, 4, 7, 7, 6, 5,
				4, 0, 3, 3, 7, 4,
				3, 2, 6, 6, 7, 3,
				4, 5, 1, 1, 0, 4
			};
			break;

		default:
			SNL_CORE_WARN("ModelConstruct::GetPrimitiveDatas: 图元类型为 None 无法生成数据!");
			break;
		}

		return std::pair<std::vector<Vertex>, std::vector<uint32_t>>(vertices, indices);
	}

}