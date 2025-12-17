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

        inline const float& GetPerspectiveFOV() const       { return m_PerspectiveFOV; }
        inline const float& GetPerspectiveNear() const      { return m_PerspectiveNear; }
        inline const float& GetPerspectiveFar() const       { return m_PerspectiveFar; }
        inline const float& GetOrthographicSize() const     { return m_OrthographicSize; }
        inline const float& GetOrthographicNear() const     { return m_OrthographicNear; }
        inline const float& GetOrthographicFar() const      { return m_OrthographicFar; }
        inline const float& GetAspect() const               { return m_Aspect; }
        inline const float& GetViewportWidth() const        { return m_ViewportWidth; }
        inline const float& GetViewportHeight() const       { return m_ViewportHeight; }

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
