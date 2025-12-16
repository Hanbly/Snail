#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Scene/Entity.h"
#include "Snail/Scene/Component.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "yaml-cpp/yaml.h"

namespace Snail {
    // ------------------------------------ 鼠标射线 -----------------------------------------------------
    struct MouseRay {
        glm::vec3 origin;    // 射线起点
        glm::vec3 direction; // 射线方向（单位向量）

        MouseRay(const float& x, const float& y, const float& width, const float& height, const glm::mat4& view, const glm::mat4& proj);

        bool Is_Cross(Entity& entity, float& outDistance) const;
    };

    // ------------------------------------ 序列化 & 反序列化 -------------------------------------------------------
    class SceneSerializer
    {
    public:
        SceneSerializer(const Refptr<Scene>& scene);

        void Serialize(const std::string& filepath);
        // void SerializeRuntime(const std::string& filepath); // 未来实现

        bool Deserialize(const std::string& filepath);
        // bool DeserializeRuntime(const std::string& filepath); // 未来实现

    private:
        // 序列化单个 Entity
        void SerializeEntity(YAML::Emitter& out, Entity entity);
        // 反序列化单个 Entity
        void DeserializeEntity(const YAML::Node& entityNode, Entity entity);

    private:
        Refptr<Scene> m_Scene;
    };

}
