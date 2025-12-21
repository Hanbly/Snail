#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Camera.h"

namespace Snail {

	enum class SceneCameraProjectionType { Perspective = 0, Orthographic = 1 };

	static std::string SceneCameraProjectionTypeToString(const SceneCameraProjectionType& type)
	{
		switch (type)
		{
		    case SceneCameraProjectionType::Perspective: return "Perspective";
		    case SceneCameraProjectionType::Orthographic: return "Orthographic";
		    default: return "None";
		}
	}

	class SceneCamera : public Camera {
    public:
        SceneCamera() = default;
        virtual ~SceneCamera() = default;

        void SetPerspective(const float& verticalFOV, const float& nearClip, const float& farClip);
        void SetOrthographic(const float& size, const float& nearClip, const float& farClip);
        void SetViewportSize(const float& width, const float& height);

        SceneCameraProjectionType GetProjectionType() const { return m_ProjectionType; }
        void SetProjectionType(SceneCameraProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }

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
        SceneCameraProjectionType m_ProjectionType = SceneCameraProjectionType::Perspective;

		float m_PerspectiveFOV = 45.0f;
        float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 50000.0f;

        float m_OrthographicSize = 10.0f;
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

        float m_Aspect = (float)1280 / 720;
        float m_ViewportWidth = 1280, m_ViewportHeight = 720;

		glm::mat4 m_ProjectionMatrix;
	};

}
