#include "SNLpch.h"

#include "SceneUtils.h"

namespace Snail {

    MouseRay::MouseRay(const float& x, const float& y, const float& width, const float& height, const glm::mat4& view, const glm::mat4& proj)
    {
        // 映射为 NDC 屏幕坐标：-1 ~ +1，经过矩阵转置的坐标结果就是NDC，现在要进行逆向操作
        float ndcX = (2.0f * x) / (float)width - 1.0f;
        float ndcY = 1.0f - (2.0f * y) / (float)height; // TODO: OpenGL的y轴反转，这里先这样写

        origin = origin = glm::vec3(glm::inverse(view)[3]);
        // 必须 矩阵 * 向量
        glm::vec4 rayEye = glm::inverse(proj) * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
        // 这一步决定了是在算“方向”
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
        // 归一化
        direction = glm::normalize(rayWorld);
    }

    bool MouseRay::Is_Cross(Entity& entity, float& outDistance) const
    {
        glm::vec3 localMin(std::numeric_limits<float>::max());
        glm::vec3 localMax(std::numeric_limits<float>::lowest());

        bool HasGeometry = false;

        if (entity.HasAllofComponent<ModelComponent>()) {
            const auto& model = entity.GetComponent<ModelComponent>().model;
            if (!model->GetMeshes().empty()) {
                localMin = model->m_AABB.min;
                localMax = model->m_AABB.max;
                HasGeometry = true;
            }
        }

        if (!HasGeometry) return false;

        // 2. 将局部 AABB 的 8 个角点变换到世界空间
        glm::mat4 transform = entity.GetComponent<TransformComponent>().GetTransform();
        glm::vec3 corners[8];
        corners[0] = glm::vec3(localMin.x, localMin.y, localMin.z);
        corners[1] = glm::vec3(localMin.x, localMin.y, localMax.z);
        corners[2] = glm::vec3(localMin.x, localMax.y, localMin.z);
        corners[3] = glm::vec3(localMin.x, localMax.y, localMax.z);
        corners[4] = glm::vec3(localMax.x, localMin.y, localMin.z);
        corners[5] = glm::vec3(localMax.x, localMin.y, localMax.z);
        corners[6] = glm::vec3(localMax.x, localMax.y, localMin.z);
        corners[7] = glm::vec3(localMax.x, localMax.y, localMax.z);
        glm::vec3 worldMin(std::numeric_limits<float>::max());
        glm::vec3 worldMax(std::numeric_limits<float>::lowest());
        for (int i = 0; i < 8; i++) {
            // 变换点
            glm::vec4 transformed = transform * glm::vec4(corners[i], 1.0f);
            glm::vec3 p = glm::vec3(transformed); // / transformed.w; // 通常 w=1
            // 重新寻找新的 AABB 边界
            worldMin = glm::min(worldMin, p);
            worldMax = glm::max(worldMax, p);
        }
        // Slab Method 检测算法
        float tmin = (worldMin.x - origin.x) / direction.x;
        float tmax = (worldMax.x - origin.x) / direction.x;

        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (worldMin.y - origin.y) / direction.y;
        float tymax = (worldMax.y - origin.y) / direction.y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin) tmin = tymin;
        if (tymax < tmax) tmax = tymax;

        float tzmin = (worldMin.z - origin.z) / direction.z;
        float tzmax = (worldMax.z - origin.z) / direction.z;

        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin) tmin = tzmin;
        if (tzmax < tmax) tmax = tzmax;

        // tmin 是相交点距离相机的距离
        outDistance = tmin;

        // 如果 tmin < 0，说明物体在相机背后，但在射线直线上
        if (outDistance < 0) return false;

        return true;
    }

     //============================================================================================================================================

    SceneSerializer::SceneSerializer(const Refptr<Scene>& scene, const Refptr<EditorCamera>& ec)
        : m_Scene(scene), m_EC(ec) {}

    // 序列化
    void SceneSerializer::Serialize(const std::string& sceneName, const std::string& filepath)
    {
        SNL_PROFILE_FUNCTION();


        YAML::Emitter out;
        out << YAML::BeginMap; // 根节点 Map
        out << YAML::Key << "Scene" << YAML::Value << sceneName;

        // --- 序列化 场景参数 ---
		out << YAML::Key << "SceneSettings";
		out << YAML::BeginMap;

        out << YAML::Key << "AmbientStrength" << YAML::Value << m_Scene->GetAmbientStrength();

		out << YAML::EndMap; // SceneSettings 结束

		// --- 序列化 EditorCamera ---
		if (m_EC) {
			out << YAML::Key << "EditorCamera";
			out << YAML::BeginMap;
			out << YAML::Key << "EditorCameraMode" << YAML::Value << EditorCameraModeToString(m_EC->GetMode());

			out << YAML::Key << "FOV" << YAML::Value << m_EC->GetFOV();
			out << YAML::Key << "Near" << YAML::Value << m_EC->GetNear();
			out << YAML::Key << "Far" << YAML::Value << m_EC->GetFar();
			out << YAML::Key << "Aspect" << YAML::Value << m_EC->GetAspect();
			out << YAML::Key << "ViewportWidth" << YAML::Value << m_EC->GetViewportWidth();
			out << YAML::Key << "ViewportHeight" << YAML::Value << m_EC->GetViewportHeight();
			out << YAML::Key << "ViewMatrix" << YAML::Value << m_EC->GetViewMatrix();
			out << YAML::Key << "Position" << YAML::Value << m_EC->GetPosition();

			out << YAML::Key << "DirvecFront" << YAML::Value << m_EC->GetFront();
			out << YAML::Key << "DirvecRight" << YAML::Value << m_EC->GetRight();
			out << YAML::Key << "DirvecUp" << YAML::Value << m_EC->GetUp();
			out << YAML::Key << "DirvecWorldUp" << YAML::Value << m_EC->GetWorldUp();
			out << YAML::Key << "EulerPitch" << YAML::Value << m_EC->GetPitch();
			out << YAML::Key << "DirvecYaw" << YAML::Value << m_EC->GetYaw();

			out << YAML::Key << "FocalPoint" << YAML::Value << m_EC->GetFocalPoint();
			out << YAML::Key << "Distance" << YAML::Value << m_EC->GetDistance();
			out << YAML::Key << "RotateSensitivity" << YAML::Value << m_EC->GetRotateSensitivity();
			out << YAML::Key << "MoveSensitivity" << YAML::Value << m_EC->GetMoveSensitivity();
			out << YAML::Key << "RotateSpeed" << YAML::Value << m_EC->GetRotateSpeed();
			out << YAML::Key << "MoveSpeed" << YAML::Value << m_EC->GetMoveSpeed();
			out << YAML::Key << "ZoomSpeed" << YAML::Value << m_EC->GetZoomSpeed();

			out << YAML::EndMap; // EditorCamera Map 结束
		}

		// 反向遍历 Registry 中的所有 Entity （正向遍历会导致读取实体的顺序颠倒）
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        auto view = m_Scene->GetRegistry().view<entt::entity>();
        for (auto it = view.rbegin(), last = view.rend(); it != last; ++it) {
			Entity entity{ *it, m_Scene.get() };
			if (!entity.IsValid()) continue; // 跳过无效实体
			SerializeEntity(out, entity);
        }

        out << YAML::EndSeq; // Entity Seq 结束
        out << YAML::EndMap; // 根节点 Map 结束

        // 保存到文件
        std::ofstream fout(filepath);
        if (fout.is_open()) {
            fout << out.c_str();
            fout.close();
            SNL_CORE_INFO("场景已成功保存到: {0}", filepath);
        }
        else {
            SNL_CORE_ERROR("无法打开文件进行场景保存: {0}", filepath);
        }
    }

    // 序列化单个 Entity 的具体逻辑
    void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity entity)
    {
        out << YAML::BeginMap; // Entity Map
        out << YAML::Key << "Entity";
        out << YAML::Value << entity.GetUUID();

        // 序列化 TagComponent
        if (entity.HasAllofComponent<TagComponent>())
        {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "Tag" << YAML::Value << entity.GetComponent<TagComponent>().name;
            out << YAML::EndMap;
        }

        // --- 序列化 TransformComponent ---
        if (entity.HasAllofComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;
            const auto& transform = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Position" << YAML::Value << transform.position;
            out << YAML::Key << "Rotation" << YAML::Value << transform.rotation;
            out << YAML::Key << "Scale" << YAML::Value << transform.scale;
            out << YAML::EndMap;
        }

		// 序列化 SkyboxComponent
		if (entity.HasAllofComponent<SkyboxComponent>())
		{
			out << YAML::Key << "SkyboxComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Active" << YAML::Value << entity.GetComponent<SkyboxComponent>().Active;
			out << YAML::EndMap;
		}

        // --- 序列化 CameraComponent ---
        if (!m_EC && entity.HasAllofComponent<CameraComponent>())
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap;
            const auto& cameraComponent = entity.GetComponent<CameraComponent>();
            out << YAML::Key << "Primary" << YAML::Value << cameraComponent.primary;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.fixedAspectRatio;

            // 序列化 SceneCamera 内部参数
            out << YAML::Key << "SceneCamera";
            out << YAML::BeginMap;
            const auto& sceneCamera = cameraComponent.camera;
            out << YAML::Key << "ProjectionType" << YAML::Value << SceneCameraProjectionTypeToString(sceneCamera.GetProjectionType());

             // 根据类型序列化不同参数
             if (sceneCamera.GetProjectionType() == SceneCameraProjectionType::Perspective) {
				 out << YAML::Key << "PerspectiveFOV" << YAML::Value << sceneCamera.GetPerspectiveFOV();
				 out << YAML::Key << "PerspectiveNear" << YAML::Value << sceneCamera.GetPerspectiveNear();
				 out << YAML::Key << "PerspectiveFar" << YAML::Value << sceneCamera.GetPerspectiveFar();
             }
             else if (sceneCamera.GetProjectionType() == SceneCameraProjectionType::Orthographic) {
				 out << YAML::Key << "OrthographicSize" << YAML::Value << sceneCamera.GetOrthographicSize();
				 out << YAML::Key << "OrthographicNear" << YAML::Value << sceneCamera.GetOrthographicNear();
				 out << YAML::Key << "OrthographicFar" << YAML::Value << sceneCamera.GetOrthographicFar();
             }
			 out << YAML::Key << "Aspect" << YAML::Value << sceneCamera.GetAspect();
			 out << YAML::Key << "ViewportWidth" << YAML::Value << sceneCamera.GetViewportWidth();
			 out << YAML::Key << "ViewportHeight" << YAML::Value << sceneCamera.GetViewportHeight();

             out << YAML::EndMap; // SceneCamera Map 结束
             out << YAML::EndMap; // CameraComponent Map 结束
         }

        // --- 序列化 PointLightComponent ---
        if (entity.HasAllofComponent<PointLightComponent>())
        {
            out << YAML::Key << "PointLightComponent";
            out << YAML::BeginMap;
            const auto& lightComponent = entity.GetComponent<PointLightComponent>();
            out << YAML::Key << "Color" << YAML::Value << lightComponent.color;
            out << YAML::Key << "Intensity" << YAML::Value << lightComponent.intensity;
            out << YAML::EndMap;
        }

        // --- 序列化 ModelComponent ---
		if (entity.HasAllofComponent<ModelComponent>())
		{
			out << YAML::Key << "ModelComponent";
			out << YAML::BeginMap;

			const auto& modelComponent = entity.GetComponent<ModelComponent>();

			// 处理模型资源
			if (modelComponent.model) {
				bool isImported = modelComponent.model->IsImported();
				out << YAML::Key << "IsImported" << YAML::Value << isImported;
				out << YAML::Key << "ShaderPath" << YAML::Value << modelComponent.model->GetShaderPath();
				if (isImported) {
					// --- 外部导入模型 (FBX/OBJ) ---
					out << YAML::Key << "FilePath" << YAML::Value << modelComponent.model->GetFullPath();
				}
				else {
					// --- 内部图元 (Cube/Sphere/Plane) ---
					out << YAML::Key << "Meshes" << YAML::Value << YAML::BeginSeq; // 开始 Mesh 列表

					for (auto& mesh : modelComponent.model->GetMeshes()) {
						out << YAML::BeginMap; // 开始单个 Mesh

						// 图元类型
                        out << YAML::Key << "PrimitiveType" << YAML::Value << PrimitiveTypeToString(mesh->GetPrimitiveType());

						// 纹理序列化
						out << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq; // 开始 Textures 列表

						auto textures = mesh->GetTextures();
						for (const auto& texture : textures) {
							out << YAML::BeginMap;

							out << YAML::Key << "DimType" << YAML::Value << TextureTypeToString(texture->GetType());
							out << YAML::Key << "Usage" << YAML::Value << TextureUsageToString(texture->GetUsage());							
							out << YAML::Key << "Paths" << YAML::Value << texture->GetPath(); // 一律存入路径数组

							out << YAML::EndMap;
						}
						out << YAML::EndSeq; // 结束 Textures

						// 不序列化 Vertices 和 Indices
						// 必须确保数据量极小，否则就会出现乱码
						// out << YAML::Key << "VertexCount" << mesh->GetVertices().size();
						// out << YAML::Key << "IndexCount" << mesh->GetIndices().size();

						out << YAML::EndMap; // 结束单个 Mesh
					}
					out << YAML::EndSeq; // 结束 Mesh 列表
				}
			}
			// 通用属性
			out << YAML::Key << "Visible" << YAML::Value << modelComponent.visible;
			out << YAML::Key << "EdgeEnable" << YAML::Value << modelComponent.edgeEnable;

			out << YAML::EndMap; // 结束 ModelComponent
		}

        // TODO: 以后继续添加其他 Component 的序列化

        out << YAML::EndMap; // Entity Map 结束
    }

	// --- 反序列化 ---
	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		SNL_PROFILE_FUNCTION();

		std::ifstream infile(filepath);
		if (!infile.is_open()) {
			SNL_CORE_ERROR("无法打开文件进行场景加载: {0}", filepath);
			return false;
		}

		std::stringstream ss;
		ss << infile.rdbuf();
		infile.close();

		YAML::Node data = YAML::Load(ss.str());
		if (!data["Scene"]) {
			SNL_CORE_ERROR("场景文件格式错误或为空: {0}", filepath);
			return false;
		}

		std::string sceneName = data["Scene"].as<std::string>();
		// m_Scene->SetName(sceneName); // 如果 Scene 有 SetName 接口

		// 反序列化场景全局设置
		auto settingsNode = data["SceneSettings"];
		if (settingsNode) {
			if (settingsNode["AmbientStrength"])
				m_Scene->SetAmbientStrength(settingsNode["AmbientStrength"].as<float>());
		}

		// 反序列化编辑器相机 (EditorCamera)
		auto editorCameraNode = data["EditorCamera"];
		if (m_EC && editorCameraNode) {

			m_EC->SetMode(StringToEditorCameraMode(editorCameraNode["EditorCameraMode"].as<std::string>()));

			m_EC->SetDistance(editorCameraNode["Distance"].as<float>());
			m_EC->SetFocalPoint(editorCameraNode["FocalPoint"].as<glm::vec3>());
			m_EC->SetPosition(editorCameraNode["Position"].as<glm::vec3>());
			m_EC->SetPitch(editorCameraNode["EulerPitch"].as<float>());
			m_EC->SetYaw(editorCameraNode["DirvecYaw"].as<float>());

			m_EC->SetViewportSize(editorCameraNode["ViewportWidth"].as<float>(), editorCameraNode["ViewportHeight"].as<float>());
		}

		// 反序列化实体 (Entities)
		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entityNode : entities)
			{
				// 获取 UUID 和 Tag (Name)
				std::string uuid_str = entityNode["Entity"].as<std::string>();

				boost::uuids::uuid uuid = boost::lexical_cast<boost::uuids::uuid>(uuid_str);

				std::string name;
				auto tagComponent = entityNode["TagComponent"];
				if (tagComponent) 
					name = tagComponent["Tag"].as<std::string>();

				Entity deserializedEntity = m_Scene->CreateEntityWithUuid(uuid, name);

				// --- TransformComponent ---
				auto transformComponent = entityNode["TransformComponent"];
				if (transformComponent) {
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.position = transformComponent["Position"].as<glm::vec3>();
					tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.scale = transformComponent["Scale"].as<glm::vec3>();
				}
				else {
					// 如果 YAML 中没有 Transform (例如 Skybox )，则移除 TransformComponent
					deserializedEntity.RemoveComponent<TransformComponent>();
				}

				// --- SkyboxComponent ---
				auto skyboxComponent = entityNode["SkyboxComponent"];
				if (skyboxComponent) {
					auto& sc = deserializedEntity.AddComponent<SkyboxComponent>();
					sc.Active = skyboxComponent["Active"].as<bool>();
				}

				// --- CameraComponent ---
				auto cameraComponent = entityNode["CameraComponent"];
				if (!m_EC && cameraComponent)
				{
					//out << YAML::Key << "CameraComponent";
					//out << YAML::BeginMap;
					//const auto& cameraComponent = entity.GetComponent<CameraComponent>();
					//out << YAML::Key << "Primary" << YAML::Value << cameraComponent.primary;
					//out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.fixedAspectRatio;

					//// 序列化 SceneCamera 内部参数
					//out << YAML::Key << "SceneCamera";
					//out << YAML::BeginMap;
					//const auto& sceneCamera = cameraComponent.camera;
					//out << YAML::Key << "ProjectionType" << YAML::Value << SceneCameraProjectionTypeToString(sceneCamera.GetProjectionType());

					//// 根据类型序列化不同参数
					//if (sceneCamera.GetProjectionType() == SceneCameraProjectionType::Perspective) {
					//	out << YAML::Key << "PerspectiveFOV" << YAML::Value << sceneCamera.GetPerspectiveFOV();
					//	out << YAML::Key << "PerspectiveNear" << YAML::Value << sceneCamera.GetPerspectiveNear();
					//	out << YAML::Key << "PerspectiveFar" << YAML::Value << sceneCamera.GetPerspectiveFar();
					//}
					//else if (sceneCamera.GetProjectionType() == SceneCameraProjectionType::Orthographic) {
					//	out << YAML::Key << "OrthographicSize" << YAML::Value << sceneCamera.GetOrthographicSize();
					//	out << YAML::Key << "OrthographicNear" << YAML::Value << sceneCamera.GetOrthographicNear();
					//	out << YAML::Key << "OrthographicFar" << YAML::Value << sceneCamera.GetOrthographicFar();
					//}
					//out << YAML::Key << "Aspect" << YAML::Value << sceneCamera.GetAspect();
					//out << YAML::Key << "ViewportWidth" << YAML::Value << sceneCamera.GetViewportWidth();
					//out << YAML::Key << "ViewportHeight" << YAML::Value << sceneCamera.GetViewportHeight();

					//out << YAML::EndMap; // SceneCamera Map 结束
					//out << YAML::EndMap; // CameraComponent Map 结束
				}

				// --- PointLightComponent ---
				auto pointLightComponent = entityNode["PointLightComponent"];
				if (pointLightComponent) {
					auto& plc = deserializedEntity.AddComponent<PointLightComponent>();
					plc.color = pointLightComponent["Color"].as<glm::vec4>();
					plc.intensity = pointLightComponent["Intensity"].as<float>();
				}

				// --- ModelComponent ---
				auto modelComponent = entityNode["ModelComponent"];
				if (modelComponent)
				{
					// 读取基础属性
					bool visible = modelComponent["Visible"].as<bool>();
					bool edgeEnable = modelComponent["EdgeEnable"].as<bool>();
					bool isImported = modelComponent["IsImported"].as<bool>();
					std::string shaderPath = modelComponent["ShaderPath"].as<std::string>();

					// 加载 Shader
					Refptr<Shader> shader = ShaderLibrary::Load(shaderPath);

					Refptr<Model> model = nullptr;

					if (isImported)	{
						// --- 导入的外部模型 (FBX/OBJ) ---
						std::string filePath = modelComponent["FilePath"].as<std::string>();
						// 调用 Model 构造函数加载文件
						model = std::make_shared<Model>(shader, filePath); // TODO: 设置缓存，并先检查缓存
					}
					else {
						// --- 图元 (Cube/Sphere) + 纹理重建 ---
						auto meshesNode = modelComponent["Meshes"];
						if (meshesNode && meshesNode.size() > 0) {
							// 简化：目前 Light/Cube 只有一个 Mesh
							auto meshNode = meshesNode[0];

							std::string primStr = meshNode["PrimitiveType"].as<std::string>();
							PrimitiveType primType = StringToPrimitiveType(primStr);

							// 重建纹理列表
							std::vector<Refptr<Texture>> textureDataList;
							auto texturesNode = meshNode["Textures"];
							if (texturesNode) {
								for (auto texNode : texturesNode) {
									std::string usage = texNode["Usage"].as<std::string>(); // "texture_diffuse" ...

									if (texNode["Paths"]) {
										auto paths = texNode["Paths"].as<std::vector<std::string>>();
										// 内部判断是单路径还是多路径 (比如 Texture2D vs CubeMap)
										if (auto texture = TextureLibrary::Load(paths, StringToTextureUsage(usage))) {
											textureDataList.push_back(texture);
										}
									}									
								}
							}

							// 调用 Model 构造函数创建图元 
							// (不需要传入 vertices/indices，内部根据 PrimitiveType 生成)
							model = std::make_shared<Model>(primType, shader, textureDataList);
						}
					}

					if (model) {
						auto& mc = deserializedEntity.AddComponent<ModelComponent>(model);
						mc.visible = visible;
						mc.edgeEnable = edgeEnable;
					}
				}
			}
		}

		SNL_CORE_INFO("场景已成功加载: {0}", filepath);
		return true;
	}

}
