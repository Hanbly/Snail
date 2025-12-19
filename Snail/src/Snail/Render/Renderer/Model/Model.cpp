#include "SNLpch.h"

#include "Snail/Render/Renderer/Renderer3D.h"

#include "Model.h"

namespace Snail {

	Model::Model(const Refptr<Mesh>& mesh)
		: m_IsImported(false)
	{
		if (mesh->GetVAO() && mesh->GetMaterial()) {

			m_Shader = mesh->GetMaterial()->GetShader();

			m_AABB.min = glm::vec3(std::numeric_limits<float>::max());
			m_AABB.max = glm::vec3(std::numeric_limits<float>::lowest());
			ConsiderMeshAABB(mesh);

			m_Meshes.push_back(mesh);
		}
	}

	Model::Model(const PrimitiveType& type, const Refptr<Shader>& shader, const std::vector<Refptr<Texture>>& textures, const glm::mat4& localTransform)
		: m_Shader(shader), m_IsImported(false)
	{
		auto [vertices, indices] = GetPrimitiveDatas(type);

		auto& mesh = std::make_shared<Mesh>(type, vertices, indices, shader, textures, localTransform);

		m_AABB.min = glm::vec3(std::numeric_limits<float>::max());
		m_AABB.max = glm::vec3(std::numeric_limits<float>::lowest());
		ConsiderMeshAABB(mesh);

		m_Meshes.push_back(mesh);
	}

	Model::Model(const PrimitiveType& type, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Refptr<Shader>& shader, const std::vector<Refptr<Texture>>& textures, const glm::mat4& localTransform)
		: m_Shader(shader), m_IsImported(false)
	{
		auto& mesh = std::make_shared<Mesh>(type, vertices, indices, shader, textures, localTransform);

		m_AABB.min = glm::vec3(std::numeric_limits<float>::max());
		m_AABB.max = glm::vec3(std::numeric_limits<float>::lowest());
		ConsiderMeshAABB(mesh);

		m_Meshes.push_back(mesh);
	}

	Model::Model(const Refptr<Shader>& shader, const std::string& objPath)
		: m_Shader(shader), m_IsImported(true)
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
		m_FullPath = path;
		//      0 - - - - - - - - - -/ 后面的剔除，取assets/models/sponza
		// 只取 0 到 从后往前数第一个'/'位置 的字符串，find_last_of返回首次找到'/'的索引
		m_Directory = path.substr(0, path.find_last_of('/'));

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

		Refptr<Mesh> resultMesh = std::make_shared<Mesh>(PrimitiveType::None, vertices, indices, m_Shader, textures, localTransformation);

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
		switch (type)
		{
			case PrimitiveType::Cube: // 六面分别定义的立方体（能更好地显示纹理）
				vertices = {
					// 格式需对应 Vertex 结构体定义: { Position, Normal, TexCoord }
					// 1. 前面 (Front Face) - Z = 0.5f
					// Pos                          // Normal           // UV
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
					// 只需要位置 (Position)，Normal 和 UV 在 Skybox shader 中通常用不到，设为 0
					// ---------------------------------------------------------------------
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
					// Front face
					0, 1, 2,
					2, 3, 0,
					// Right face
					1, 5, 6,
					6, 2, 1,
					// Back face
					5, 4, 7,
					7, 6, 5,
					// Left face
					4, 0, 3,
					3, 7, 4,
					// Top face
					3, 2, 6,
					6, 7, 3,
					// Bottom face
					4, 5, 1,
					1, 0, 4
				};
				break;
			case PrimitiveType::Sphere: SNL_CORE_WARN("ModelConstruct::GetPrimitiveDatas: 目前不支持 图元类型为 Sphere !"); break;
			case PrimitiveType::Plane: SNL_CORE_WARN("ModelConstruct::GetPrimitiveDatas: 目前不支持 图元类型为 Plane !"); break;
			default: SNL_CORE_WARN("ModelConstruct::GetPrimitiveDatas: 图元类型为 None 无法生成数据!");
		}

		return std::pair<std::vector<Vertex>, std::vector<uint32_t>>(vertices, indices);
	}

}