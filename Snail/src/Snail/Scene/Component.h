#pragma once

#include <string>

#include "Snail/Render/Renderer/Model/Model.h"
#include "Snail/Render/Renderer/Camera/Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Snail {

	struct TagComponent {
		std::string name;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& name)
			: name(name) {}
	};

	struct TransformComponent {
		glm::mat4 transform;
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			: transform(transform) {
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

    struct MeshComponent {
        Refptr<Mesh> mesh;
        bool visible = true;
        bool edgeEnable = false;

        MeshComponent() = default;
        MeshComponent(const Refptr<Mesh>& mesh) : mesh(mesh) {}
    };

    struct CameraComponent {
        Camera camera;
        bool primary = true; // 标记是否为主相机

        CameraComponent() = default;
        CameraComponent(const Camera& camera) 
            : camera(camera) {}
    };

    // 点光源组件
    struct PointLightComponent {
        glm::vec4 color = glm::vec4(1.0f);
        float intensity = 1.0f;

        PointLightComponent() = default;
        PointLightComponent(const PointLightComponent&) = default;
        PointLightComponent(const glm::vec4& color, const float& intensity)
            : color(color), intensity(intensity) {}
    };

}