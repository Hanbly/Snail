#include "SNLpch.h"

#include "Camera.h"

namespace Snail {

	Camera::Camera(const float& fov, const float& aspect, const glm::vec3& position)
		: m_FOV(fov), m_Aspect(aspect), m_Position(position)
	{
		m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
		m_Front = glm::vec3(0.0f, 0.0f, -1.0f); // 默认看向 -Z
		m_Right = glm::normalize(glm::cross(m_Front, m_Up));

		// 初始角度：看向 -Z 时，Yaw 通常是 -90度
		m_Yaw = -90.0f;
		m_Pitch = 0.0f;

		m_ViewMatrix = glm::mat4(1.0f);
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_Aspect, m_Near, m_Far);

		this->RecalculateMatrix();
	}

	void Camera::MoveCamera(const TranslationDirection& dir, const float& length) {
		//SNL_TRACE("ExampleLayer 调用: OnUpdate()");
		glm::vec3 upDir = glm::normalize(glm::cross(m_Right, m_Front));

		glm::vec3 vector = glm::vec3(0.0f);
		if (dir == TranslationDirection::FRONT) {
			vector = m_Front * length;
		}
		else if (dir == TranslationDirection::BACK) {
			vector = -m_Front * length;
		}
		if (dir == TranslationDirection::RIGHT) {
			vector = m_Right * length;
		}
		else if (dir == TranslationDirection::LEFT) {
			vector = -m_Right * length;
		}
		if (dir == TranslationDirection::UP) {
			vector = upDir * length;
		}
		else if (dir == TranslationDirection::DOWN) {
			vector = -upDir * length;
		}

		m_Position = m_Position + vector;
		this->RecalculateMatrix();
	}

	void Camera::RotateCamera(const float& yaw, const float& pitch)
	{
		// 1. 累加偏移量到总角度
		m_Yaw += yaw;
		m_Pitch += pitch;

		if (m_Pitch > 89.0f) m_Pitch = 89.0f;
		if (m_Pitch < -89.0f) m_Pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
		front.y = sin(glm::radians(m_Pitch));
		front.z = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));
		m_Front = glm::normalize(front);

		m_Right = glm::normalize(glm::cross(m_Front, m_Up));

		this->RecalculateMatrix();	
	}

	void Camera::RecalculateMatrix() {
		// 根据现有相机属性，对其矩阵进行重新计算

		// 参数: 眼睛位置, 目标位置, 上向量
		// -----------------------------------------------------------
		m_ViewMatrix = glm::lookAt(m_Position, m_Front + m_Position, m_Up);
	}

}