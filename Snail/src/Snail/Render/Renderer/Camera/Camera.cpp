#include "SNLpch.h"

#include "Camera.h"

namespace Snail {

	Camera::Camera(const glm::float32& fov, const glm::float32& aspect, const glm::vec3& position,
		const glm::float32& np, const glm::float32& fp)
		: m_FOV(fov), m_Aspect(aspect), m_NearPoint(np), m_FarPoint(fp), m_Position(position)
	{
		m_Front = glm::vec3(0.0f, 0.0f, 0.0f);

		m_ViewMatrix = glm::mat4(1.0f);
		m_ProjectionMatrix = glm::mat4(1.0f);

		this->RecalculateMatrix();
	}

	void Camera::MoveCamera(const TranslationDirection& dir, const float& length) {
		//SNL_TRACE("ExampleLayer 调用: OnUpdate()");
		glm::vec3 frontdDir = glm::normalize(m_Front - m_Position);
		glm::vec3 rightDir = glm::normalize(glm::cross(frontdDir, m_Up));
		glm::vec3 upDir = glm::normalize(glm::cross(rightDir, frontdDir));

		glm::vec3 vector = glm::vec3(0.0f);
		if (dir == TranslationDirection::FRONT) {
			vector = frontdDir * length;
		}
		else if (dir == TranslationDirection::BACK) {
			vector = -frontdDir * length;
		}
		if (dir == TranslationDirection::RIGHT) {
			vector = rightDir * length;
		}
		else if (dir == TranslationDirection::LEFT) {
			vector = -rightDir * length;
		}
		if (dir == TranslationDirection::UP) {
			vector = upDir * length;
		}
		else if (dir == TranslationDirection::DOWN) {
			vector = -upDir * length;
		}

		m_Position = m_Position + vector;
		// !注意! 在xy平面移动时，m_Front仍然是 glm::vec3(0.0f, 0.0f, 0.0f)，
		// 这样看到的并不是平移效果，而是一直盯着原点在移动
		m_Front = m_Front + vector;
		this->RecalculateMatrix();
	}

	void Camera::SetFront(const glm::vec3& deltaFront) {
		m_Front = m_Front + deltaFront;
		this->RecalculateMatrix();
	}

	void Camera::RecalculateMatrix() {
		// 根据现有相机属性，对其矩阵进行重新计算

		// 参数: 眼睛位置, 目标位置, 上向量
		// -----------------------------------------------------------
		m_ViewMatrix = glm::lookAt(m_Position, m_Front, m_Up);
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_Aspect, m_NearPoint, m_FarPoint);
	}

}