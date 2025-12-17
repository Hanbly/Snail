#pragma once

#include <string>

#include "Snail/Render/Renderer/Model/Model.h"
#include "Snail/Render/Renderer/Camera/SceneCamera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"

namespace Snail {

    struct UUIDComponent {
        boost::uuids::uuid uuid;
        UUIDComponent() = default;
        UUIDComponent(const UUIDComponent&) = default;
        UUIDComponent(const boost::uuids::uuid& uuid)
            : uuid(uuid) {}
        operator std::string() const { return boost::uuids::to_string(uuid); }
    };

	struct TagComponent {
		std::string name;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& name)
			: name(name) {}
        operator std::string() const { return name; }
	};

	struct TransformComponent {
        glm::vec3 position =    { 0, 0, 0 };
        glm::vec3 rotation =    { 0, 0, 0 };
        glm::vec3 scale =       { 1, 1, 1 };
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
			: position(position), rotation(rotation), scale(scale) {}

        const glm::mat4 GetTransform() const
        {
            glm::mat4 transform =   glm::translate(glm::mat4(1.0f), position);
            transform =             glm::rotate(transform, glm::radians(rotation.x), { 1, 0, 0 });
            transform =             glm::rotate(transform, glm::radians(rotation.y), { 0, 1, 0 });
            transform =             glm::rotate(transform, glm::radians(rotation.z), { 0, 0, 1 });
            transform =             glm::scale(transform, scale);

            return transform;
        }
	};

    struct SkyboxComponent {
        bool Active = true;
	};

	// 相机组件
	struct CameraComponent {
		SceneCamera camera;
		bool primary = true;            // 是否为主相机
		bool fixedAspectRatio = false;  // 是否不更新投影矩阵

		CameraComponent() = default;
		CameraComponent(const SceneCamera& camera)
			: camera(camera) {
		}
	};

	// 点光源组件
	struct PointLightComponent {
		glm::vec4 color = glm::vec4(1.0f);
		float intensity = 1.0f;

		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
		PointLightComponent(const glm::vec4& color, const float& intensity)
			: color(color), intensity(intensity) {
		}
	};

    // 模型组件
    struct ModelComponent {
        Refptr<Model> model;
        bool visible = true;
        bool edgeEnable = false;

        ModelComponent() = default;
        ModelComponent(const Refptr<Model>& model) : model(model) {}
    };

}