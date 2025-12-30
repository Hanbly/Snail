#include "SNLpch.h"

#include "Snail/Render/Renderer/Renderer3D.h"

#include "Model.h"

namespace Snail {

	Model::Model(const Refptr<Mesh>& mesh)
		: m_IsImported(false)
	{
		if (mesh->GetVAO() && mesh->GetMaterial()) {

			m_DefaultShader = mesh->GetMaterial()->GetShader();

			m_AABB.min = glm::vec3(std::numeric_limits<float>::max());
			m_AABB.max = glm::vec3(std::numeric_limits<float>::lowest());
			ConsiderMeshAABB(mesh);

			m_Meshes.push_back(mesh);
		}
	}

	Model::Model(const PrimitiveType& type, const Refptr<Shader>& shader, const std::vector<Refptr<Texture>>& textures, const glm::mat4& localTransform)
		: m_DefaultShader(shader), m_IsImported(false), m_PrimitiveType(type)
	{
		auto [vertices, indices] = GetPrimitiveDatas(type);

		auto& mesh = std::make_shared<Mesh>(type, vertices, indices, shader, textures, localTransform);

		m_AABB.min = glm::vec3(std::numeric_limits<float>::max());
		m_AABB.max = glm::vec3(std::numeric_limits<float>::lowest());
		ConsiderMeshAABB(mesh);

		m_Meshes.push_back(mesh);
	}

	Model::Model(const PrimitiveType& type, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Refptr<Shader>& shader, const std::vector<Refptr<Texture>>& textures, const glm::mat4& localTransform)
		: m_DefaultShader(shader), m_IsImported(false), m_PrimitiveType(type)
	{
		auto& mesh = std::make_shared<Mesh>(type, vertices, indices, shader, textures, localTransform);

		m_AABB.min = glm::vec3(std::numeric_limits<float>::max());
		m_AABB.max = glm::vec3(std::numeric_limits<float>::lowest());
		ConsiderMeshAABB(mesh);

		m_Meshes.push_back(mesh);
	}

	Model::Model(const Refptr<Shader>& shader, const std::string& objPath)
		: m_DefaultShader(shader), m_IsImported(true)
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
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
			//	  分割成三角形      |      反转y轴		  |   自动生成顶点的法向量	 

		// 读取数据是否非空 & 读取数据是否完整
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			SNL_CORE_ERROR("Model Importer 读取模型错误: ERROR::ASSIMP:: '{0}'", importer.GetErrorString());
			SNL_CORE_ASSERT(false, "Model Importer 读取模型错误");
			return;
		}
		// 示例：assets/models/sponza/sponza.obj
		std::filesystem::path stdPath = path;
		m_FullPath = path;

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
			vertex.position.x = static_cast<float>(mesh->mVertices[i].x);
			vertex.position.y = static_cast<float>(mesh->mVertices[i].y);
			vertex.position.z = static_cast<float>(mesh->mVertices[i].z);

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
			if (mesh->HasTextureCoords(0))
			{
				vertex.texCoords.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
				vertex.texCoords.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
			}
			else
			{
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}
			
			vertices.push_back(vertex);
		}

		// 处理索引
		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// 处理材质（纹理）
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			std::vector<Refptr<Texture>> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, TextureUsage::Diffuse);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<Refptr<Texture>> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, TextureUsage::Specular);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			// TODO: cube 纹理
			//std::vector<Refptr<Texture>> cubeMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, TextureUsage::Cubemap);
			//textures.insert(textures.end(), cubeMaps.begin(), cubeMaps.end());
		}

		Refptr<Mesh> resultMesh = std::make_shared<Mesh>(PrimitiveType::None, vertices, indices, m_DefaultShader, textures, localTransformation);

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

	std::vector<Refptr<Texture>> Model::LoadMaterialTextures(aiMaterial* mat, const aiTextureType& type, const TextureUsage& usage)
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
			// 防御空路径
			if (str.length == 0) {
				SNL_CORE_WARN("Model: Texture path is empty!");
				continue;
			}
			std::string filename = std::string(str.C_Str());
			std::replace(filename.begin(), filename.end(), '\\', '/');
						
			Refptr<Texture> texture;

			std::filesystem::path filePath(filename);
			if (filePath.extension() != ".png" && filePath.extension() != ".jpg") {
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
		case PrimitiveType::Cube: // 六面分别定义的立方体
			vertices = {
				// 1. 前面 (Front Face) - Z = 0.5f
				{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f} },
				{ { 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {1.0f, 0.0f} },
				{ { 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {1.0f, 1.0f} },
				{ {-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {0.0f, 1.0f} },
				// 2. 右面 (Right Face) - X = 0.5f
				{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f, 0.0f},  {0.0f, 0.0f} },
				{ { 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f} },
				{ { 0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f},  {1.0f, 1.0f} },
				{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 0.0f},  {0.0f, 1.0f} },
				// 3. 后面 (Back Face) - Z = -0.5f
				{ { 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
				{ {-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
				{ {-0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },
				{ { 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },
				// 4. 左面 (Left Face) - X = -0.5f
				{ {-0.5f, -0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
				{ {-0.5f, -0.5f,  0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
				{ {-0.5f,  0.5f,  0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
				{ {-0.5f,  0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
				// 5. 上面 (Top Face) - Y = 0.5f
				{ {-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f},  {0.0f, 0.0f} },
				{ { 0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f},  {1.0f, 0.0f} },
				{ { 0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f},  {1.0f, 1.0f} },
				{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f} },
				// 6. 下面 (Bottom Face) - Y = -0.5f
				{ {-0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
				{ { 0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
				{ { 0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
				{ {-0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} }
			};
			indices = {
				0, 1, 2, 2, 3, 0,       // 前面
				4, 5, 6, 6, 7, 4,       // 右面
				8, 9, 10, 10, 11, 8,    // 后面
				12, 13, 14, 14, 15, 12, // 左面
				16, 17, 18, 18, 19, 16, // 上面
				20, 21, 22, 22, 23, 20  // 下面
			};
			break;

		case PrimitiveType::Skybox: // 天空盒
			vertices = {
				// ... (你原本的代码保持不变) ...
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

		case PrimitiveType::Sphere:
		{
			// 生成 UV Sphere
			const int X_SEGMENTS = 64; // 经度切分 (Sectors)
			const int Y_SEGMENTS = 64; // 纬度切分 (Stacks)
			const float radius = 0.5f; // 半径 0.5，直径 1.0，匹配 Cube 大小

			for (int y = 0; y <= Y_SEGMENTS; ++y)
			{
				for (int x = 0; x <= X_SEGMENTS; ++x)
				{
					float xSegment = (float)x / (float)X_SEGMENTS;
					float ySegment = (float)y / (float)Y_SEGMENTS;

					// 使用球坐标系计算位置
					// ySegment * PI 从 0 (顶) 到 PI (底)
					// xSegment * 2PI 从 0 到 2PI (绕圈)
					float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
					float yPos = std::cos(ySegment * PI);
					float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

					// 法线就是归一化的位置向量 (对于单位球)
					glm::vec3 normal = glm::vec3(xPos, yPos, zPos);

					// 应用半径
					glm::vec3 pos = normal * radius;

					// UV 坐标 (简单映射)
					glm::vec2 uv = glm::vec2(xSegment, ySegment);

					vertices.push_back({ pos, normal, uv });
				}
			}

			// 生成索引
			bool oddRow = false;
			for (int y = 0; y < Y_SEGMENTS; ++y)
			{
				// 如果处理极点可能需要优化（顶部和底部三角形），但通用逻辑如下：
				for (int x = 0; x < X_SEGMENTS; ++x)
				{
					// 每一格由两个三角形组成
					uint32_t k1 = y * (X_SEGMENTS + 1) + x;
					uint32_t k2 = k1 + X_SEGMENTS + 1;

					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);

					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
			break;
		}

		case PrimitiveType::Plane:
		{
			// XZ 平面，Y 轴朝上，中心在原点，大小 1x1
			// 与 Cube 的面大小保持一致
			float size = 0.5f;
			vertices = {
				// Pos                        // Normal           // UV
				// 左上 (Top Left) -> Z是负的还是正的取决于你的坐标系习惯，通常OpenGL里 Z+是屏幕外
				// 对应 Cube 的 Top Face
				{ {-size, 0.0f, -size},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f} }, // 0
				{ {-size, 0.0f,  size},  {0.0f, 1.0f, 0.0f},  {0.0f, 0.0f} }, // 1
				{ { size, 0.0f,  size},  {0.0f, 1.0f, 0.0f},  {1.0f, 0.0f} }, // 2
				{ { size, 0.0f, -size},  {0.0f, 1.0f, 0.0f},  {1.0f, 1.0f} }  // 3
			};

			indices = {
				0, 1, 2,
				2, 3, 0
			};
			break;
		}

		default:
			SNL_CORE_WARN("ModelConstruct::GetPrimitiveDatas: 图元类型为 None 无法生成数据!");
			break;
		}

		return std::pair<std::vector<Vertex>, std::vector<uint32_t>>(vertices, indices);
	}

}