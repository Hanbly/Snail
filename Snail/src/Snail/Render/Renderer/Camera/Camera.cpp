#include "SNLpch.h"

#include "Camera.h"

namespace Snail {

	Camera::Camera(const float& fov, const float& aspect, const glm::vec3& position)
		: m_FOV(fov), m_Aspect(aspect), m_Position(position)
	{
		m_Front = glm::vec3(0.0f, 0.0f, -1.0f); // 默认看向 -Z
		m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
		m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));

		// 初始角度：看向 -Z 时，Yaw 通常是 -90度
		m_Yaw = -90.0f;
		m_Pitch = 0.0f;

		m_ViewMatrix = glm::mat4(1.0f);
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_Aspect, m_Near, m_Far);
	}

}