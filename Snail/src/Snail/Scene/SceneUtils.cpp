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
            if (!model->GetMeshs().empty()) {
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

    //// ------------------------------------------------------------------------------------------
    //SceneSerializer::SceneSerializer(const Refptr<Scene>& scene)
    //    : m_Scene(scene) {}

    //// 序列化
    //void SceneSerializer::Serialize(const std::string& filepath)
    //{
    //    SNL_PROFILE_FUNCTION();

    //    YAML::Emitter out;
    //    out << YAML::BeginMap; // 根节点 Map
    //    out << YAML::Key << "Scene" << YAML::Value << "Untitled Scene"; // 场景名字，可动态设置
    //    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq; // Entity 列表 Seq

    //    // 遍历 Registry 中的所有 Entity
    //    m_Scene->GetRegistry().each([&](entt::entity entityID) {
    //        Entity entity{ entityID, m_Scene.get() };
    //        if (!entity.IsValid()) return; // 跳过无效实体

    //        SerializeEntity(out, entity);
    //        });

    //    out << YAML::EndSeq; // Entity Seq 结束
    //    out << YAML::EndMap; // 根节点 Map 结束

    //    // 保存到文件
    //    std::ofstream fout(filepath);
    //    if (fout.is_open()) {
    //        fout << out.c_str();
    //        fout.close();
    //        SNL_CORE_INFO("场景已成功保存到: {0}", filepath);
    //    }
    //    else {
    //        SNL_CORE_ERROR("无法打开文件进行场景保存: {0}", filepath);
    //    }
    //}

    //// 序列化单个 Entity 的具体逻辑
    //void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity entity)
    //{
    //    out << YAML::BeginMap; // Entity Map
    //    out << YAML::Key << "Entity";
    //    out << YAML::Value << (uint64_t)entity.GetUUID(); // 假设 Entity 有 UUID

    //    // 序列化 TagComponent
    //    if (entity.HasComponent<TagComponent>())
    //    {
    //        out << YAML::Key << "TagComponent";
    //        out << YAML::BeginMap;
    //        out << YAML::Key << "Tag" << YAML::Value << entity.GetComponent<TagComponent>().name;
    //        out << YAML::EndMap;
    //    }

    //    // --- 序列化 TransformComponent ---
    //    if (entity.HasComponent<TransformComponent>())
    //    {
    //        out << YAML::Key << "TransformComponent";
    //        out << YAML::BeginMap;
    //        const auto& transform = entity.GetComponent<TransformComponent>();
    //        out << YAML::Key << "Position" << YAML::Value << transform.position;
    //        out << YAML::Key << "Rotation" << YAML::Value << transform.rotation;
    //        out << YAML::Key << "Scale" << YAML::Value << transform.scale;
    //        out << YAML::EndMap;
    //    }

    //    // --- 序列化 CameraComponent ---
    //    if (entity.HasComponent<CameraComponent>())
    //    {
    //        out << YAML::Key << "CameraComponent";
    //        out << YAML::BeginMap;
    //        const auto& cameraComponent = entity.GetComponent<CameraComponent>();
    //        out << YAML::Key << "Primary" << YAML::Value << cameraComponent.primary;
    //        out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.fixedAspectRatio;

    //        // 序列化 SceneCamera 内部参数
    //        out << YAML::Key << "SceneCamera";
    //        out << YAML::BeginMap;
    //        const auto& sceneCamera = cameraComponent.camera;
    //        out << YAML::Key << "ProjectionType" << YAML::Value << (int)sceneCamera.GetProjectionType(); // 转换为 int

    //        // 根据类型序列化不同参数
    //        if (sceneCamera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
    //            // TODO: 需要 SceneCamera 提供 getter 方法来获取 FOV, Near, Far
    //            // out << YAML::Key << "PerspectiveFOV" << YAML::Value << sceneCamera.GetPerspectiveFOV();
    //            // out << YAML::Key << "PerspectiveNear" << YAML::Value << sceneCamera.GetPerspectiveNear();
    //            // out << YAML::Key << "PerspectiveFar" << YAML::Value << sceneCamera.GetPerspectiveFar();
    //        }
    //        else {
    //            // TODO: 需要 SceneCamera 提供 getter 方法来获取 Orthographic Size, Near, Far
    //            // out << YAML::Key << "OrthographicSize" << YAML::Value << sceneCamera.GetOrthographicSize();
    //            // out << YAML::Key << "OrthographicNear" << YAML::Value << sceneCamera.GetOrthographicNear();
    //            // out << YAML::Key << "OrthographicFar" << YAML::Value << sceneCamera.GetOrthographicFar();
    //        }
    //        out << YAML::EndMap; // SceneCamera Map 结束
    //        out << YAML::EndMap; // CameraComponent Map 结束
    //    }

    //    // --- 序列化 PointLightComponent ---
    //    if (entity.HasComponent<PointLightComponent>())
    //    {
    //        out << YAML::Key << "PointLightComponent";
    //        out << YAML::BeginMap;
    //        const auto& lightComponent = entity.GetComponent<PointLightComponent>();
    //        out << YAML::Key << "Color" << YAML::Value << lightComponent.color;
    //        out << YAML::Key << "Intensity" << YAML::Value << lightComponent.intensity;
    //        out << YAML::EndMap;
    //    }

    //    // --- 序列化 ModelComponent ---
    //    if (entity.HasComponent<ModelComponent>())
    //    {
    //        out << YAML::Key << "ModelComponent";
    //        out << YAML::BeginMap;
    //        const auto& modelComponent = entity.GetComponent<ModelComponent>();
    //        if (modelComponent.model) {
    //            // TODO: 序列化模型路径 (string), 而不是 Refptr<Model>
    //            // 假设 Model 类有一个 GetPath()
    //            // out << YAML::Key << "Path" << YAML::Value << modelComponent.model->GetPath();
    //            // 暂时跳过，因为 Asset System 还没建立
    //            SNL_CORE_WARN("ModelComponent 路径暂未序列化.");
    //        }
    //        out << YAML::Key << "Visible" << YAML::Value << modelComponent.visible;
    //        out << YAML::Key << "EdgeEnable" << YAML::Value << modelComponent.edgeEnable;
    //        out << YAML::EndMap;
    //    }

    //    // TODO: 以后继续添加其他 Component 的序列化

    //    out << YAML::EndMap; // Entity Map 结束
    //}

    //// --- 反序列化 ---
    //bool SceneSerializer::Deserialize(const std::string& filepath)
    //{
    //    SNL_PROFILE_FUNCTION();

    //    std::ifstream infile(filepath);
    //    if (!infile.is_open()) {
    //        SNL_CORE_ERROR("无法打开文件进行场景加载: {0}", filepath);
    //        return false;
    //    }

    //    std::stringstream ss;
    //    ss << infile.rdbuf();
    //    infile.close();

    //    YAML::Node data = YAML::Parse(ss.str());
    //    if (!data["Entities"]) {
    //        SNL_CORE_ERROR("场景文件格式错误: 缺少 'Entities' 节点");
    //        return false;
    //    }

    //    // 清空当前场景（为了重新加载）
    //    // m_Scene->Clear(); // 需要 Scene 类提供 Clear() 方法

    //    auto entities = data["Entities"];
    //    if (entities)
    //    {
    //        for (auto entityNode : entities)
    //        {
    //            // 1. 创建 Entity
    //            // TODO: 这里需要先解析 Entity ID (UUID)
    //            uint64_t uuid = entityNode["Entity"].as<uint64_t>();
    //            Entity entity = m_Scene->CreateEntity("Deserialized"); // 临时名字，后面会被 TagComponent 覆盖

    //            // 2. 反序列化 Component
    //            DeserializeEntity(entityNode, entity);
    //        }
    //    }

    //    SNL_CORE_INFO("场景已成功加载: {0}", filepath);
    //    return true;
    //}

    //// 反序列化单个 Entity 的具体逻辑
    //void SceneSerializer::DeserializeEntity(const YAML::Node& entityNode, Entity entity)
    //{
    //    // --- 反序列化 TagComponent ---
    //    auto tagComponentNode = entityNode["TagComponent"];
    //    if (tagComponentNode)
    //    {
    //        std::string tag = tagComponentNode["Tag"].as<std::string>();
    //        // 如果 TagComponent 不存在，则添加，否则修改
    //        if (entity.HasComponent<TagComponent>()) {
    //            entity.GetComponent<TagComponent>().name = tag;
    //        }
    //        else {
    //            entity.AddComponent<TagComponent>(tag);
    //        }
    //    }

    //    // --- 反序列化 TransformComponent ---
    //    auto transformComponentNode = entityNode["TransformComponent"];
    //    if (transformComponentNode)
    //    {
    //        glm::vec3 position = transformComponentNode["Position"].as<glm::vec3>();
    //        glm::vec3 rotation = transformComponentNode["Rotation"].as<glm::vec3>();
    //        glm::vec3 scale = transformComponentNode["Scale"].as<glm::vec3>();

    //        if (entity.HasComponent<TransformComponent>()) {
    //            auto& transform = entity.GetComponent<TransformComponent>();
    //            transform.position = position;
    //            transform.rotation = rotation;
    //            transform.scale = scale;
    //        }
    //        else {
    //            entity.AddComponent<TransformComponent>(position, rotation, scale);
    //        }
    //    }

    //    // --- 反序列化 CameraComponent ---
    //    auto cameraComponentNode = entityNode["CameraComponent"];
    //    if (cameraComponentNode)
    //    {
    //        bool primary = cameraComponentNode["Primary"].as<bool>();
    //        bool fixedAspectRatio = cameraComponentNode["FixedAspectRatio"].as<bool>();

    //        auto& cameraComp = entity.AddComponent<CameraComponent>(); // 自动创建
    //        cameraComp.primary = primary;
    //        cameraComp.fixedAspectRatio = fixedAspectRatio;

    //        auto sceneCameraNode = cameraComponentNode["SceneCamera"];
    //        if (sceneCameraNode) {
    //            SceneCamera::ProjectionType type = (SceneCamera::ProjectionType)sceneCameraNode["ProjectionType"].as<int>();
    //            cameraComp.camera.SetProjectionType(type);

    //            if (type == SceneCamera::ProjectionType::Perspective) {
    //                // TODO: 获取参数并设置 SceneCamera
    //                // cameraComp.camera.SetPerspective(
    //                //     sceneCameraNode["PerspectiveFOV"].as<float>(),
    //                //     sceneCameraNode["PerspectiveNear"].as<float>(),
    //                //     sceneCameraNode["PerspectiveFar"].as<float>()
    //                // );
    //            }
    //            else {
    //                // TODO: 获取参数并设置 SceneCamera
    //                // cameraComp.camera.SetOrthographic(
    //                //     sceneCameraNode["OrthographicSize"].as<float>(),
    //                //     sceneCameraNode["OrthographicNear"].as<float>(),
    //                //     sceneCameraNode["OrthographicFar"].as<float>()
    //                // );
    //            }
    //            // 场景加载后，需要根据当前视口更新 Aspect Ratio
    //            // cameraComp.camera.SetViewportSize(...); // 这会在 SnailEditorLayer 的 OnUpdate/OnImGuiRender 中处理
    //        }
    //    }

    //    // --- 反序列化 PointLightComponent ---
    //    auto pointLightComponentNode = entityNode["PointLightComponent"];
    //    if (pointLightComponentNode)
    //    {
    //        glm::vec4 color = pointLightComponentNode["Color"].as<glm::vec4>();
    //        float intensity = pointLightComponentNode["Intensity"].as<float>();

    //        if (entity.HasComponent<PointLightComponent>()) {
    //            auto& plc = entity.GetComponent<PointLightComponent>();
    //            plc.color = color;
    //            plc.intensity = intensity;
    //        }
    //        else {
    //            entity.AddComponent<PointLightComponent>(color, intensity);
    //        }
    //    }

    //    // --- 反序列化 ModelComponent ---
    //    auto modelComponentNode = entityNode["ModelComponent"];
    //    if (modelComponentNode)
    //    {
    //        bool visible = modelComponentNode["Visible"].as<bool>(true); // 默认值
    //        bool edgeEnable = modelComponentNode["EdgeEnable"].as<bool>(false); // 默认值

    //        auto modelPathNode = modelComponentNode["Path"];
    //        if (modelPathNode) {
    //            std::string modelPath = modelPathNode.as<std::string>();
    //            // TODO: Asset System needed! How to load model based on path?
    //            // For now, assume Model loading is handled elsewhere or manually.
    //            // For demonstration, we'll just set the flags and expect model to be loaded manually.
    //            SNL_CORE_WARN("ModelComponent Path '{0}' deserialization needs Asset System.", modelPath);

    //            // 假设模型加载成功后，会创建 ModelComponent
    //            // entity.AddComponent<ModelComponent>(AssetManager::LoadModel(modelPath));
    //            // entity.GetComponent<ModelComponent>().visible = visible;
    //            // entity.GetComponent<ModelComponent>().edgeEnable = edgeEnable;
    //        }
    //        else {
    //            // 如果没有路径，只设置其他参数
    //            if (entity.HasComponent<ModelComponent>()) {
    //                auto& mc = entity.GetComponent<ModelComponent>();
    //                mc.visible = visible;
    //                mc.edgeEnable = edgeEnable;
    //            }
    //            else {
    //                // 如果没有模型component，但有这些flag，可能意味着这个组件只是标记，没有实际模型
    //                // 或者是一个占位符，暂时不创建。
    //            }
    //        }
    //    }
    //}

}