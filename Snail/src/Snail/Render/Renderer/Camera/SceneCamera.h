#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Camera.h"

namespace Snail {

	class SceneCamera : public Camera {
    public:
        enum class ProjectionType { Perspective = 0, Orthographic = 1 };
    public:
        SceneCamera() = default;
        virtual ~SceneCamera() = default;

        void SetPerspective(const float& verticalFOV, const float& nearClip, const float& farClip);
        void SetOrthographic(const float& size, const float& nearClip, const float& farClip);
        void SetViewportSize(const float& width, const float& height);

        ProjectionType GetProjectionType() const { return m_ProjectionType; }
        void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }

    private:
        void RecalculateProjection();
    private:
        ProjectionType m_ProjectionType = ProjectionType::Perspective;

		float m_PerspectiveFOV = 45.0f;
        float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 5000.0f;

        float m_OrthographicSize = 10.0f;
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

        float m_Aspect = (float)1280 / 720;
        float m_ViewportWidth = 1280, m_ViewportHeight = 720;

		glm::mat4 m_ProjectionMatrix;
	};

}
