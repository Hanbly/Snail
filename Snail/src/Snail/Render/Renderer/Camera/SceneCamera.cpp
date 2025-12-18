#include "SNLpch.h"

#include "SceneCamera.h"

namespace Snail {

	void SceneCamera::SetPerspective(const float& verticalFOV, const float& nearClip, const float& farClip)
	{
		// 防止FOV过小或过大
		if (verticalFOV < 1.0f) m_PerspectiveFOV = 1.0f;
		if (verticalFOV > 90.0f) m_PerspectiveFOV = 90.0f;

		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(const float& size, const float& nearClip, const float& farClip)
	{
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
	}

	void SceneCamera::SetViewportSize(const float& width, const float& height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
		m_Aspect = width / height;

		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		if (m_ProjectionType == SceneCameraProjectionType::Perspective) {
			m_ProjectionMatrix = glm::perspective(glm::radians(m_PerspectiveFOV), m_Aspect, m_PerspectiveNear, m_PerspectiveFar);
		}
		else if (m_ProjectionType == SceneCameraProjectionType::Orthographic) {
			m_ProjectionMatrix = glm::ortho(0.0f, m_OrthographicSize, 0.0f, m_OrthographicSize, m_OrthographicNear, m_OrthographicFar);
		}
	}

}
