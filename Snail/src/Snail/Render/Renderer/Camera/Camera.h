#pragma once

#include "Snail/Core.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Snail {

	class Camera {
	private:
		glm::float32 m_FOV;
		glm::float32 m_Aspect;
		glm::float32 m_NearPoint;
		glm::float32 m_FarPoint;

		glm::vec3 m_Position;
		glm::vec3 m_Front;

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;

	public:
		Camera(const glm::float32& fov, const glm::float32& aspect, const glm::vec3& position,
			   const glm::float32& np = 0.1f, const glm::float32& fp = 100.0f)
			: m_FOV(fov), m_Aspect(aspect), m_NearPoint(np), m_FarPoint(fp), m_Position(position)
		{
			m_Front = glm::vec3(0.0f, 0.0f, 0.0f);

			m_ViewMatrix = glm::mat4(1.0f);
			m_ProjectionMatrix = glm::mat4(1.0f);

			this->RecalculateMatrix();
		}

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const glm::vec3& GetFront() const { return m_Front; }
		inline void SetPosition(const glm::vec3& position) {
			m_Position = position;
			this->RecalculateMatrix();
		}
		inline void SetFront(const glm::vec3& front) {
			m_Front = front;
			this->RecalculateMatrix();
		}

		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

		inline void SetViewMatrix(const glm::mat4& mat4) { m_ViewMatrix = mat4; }
		inline void SetProjectionMatrix(const glm::mat4& mat4) { m_ProjectionMatrix = mat4; }

		void RecalculateMatrix() {
			// 根据现有相机属性，对其矩阵进行重新计算

			// 参数: 眼睛位置, 目标位置, 上向量
			// -----------------------------------------------------------
			// 假设你的 m_Direction 是相机看向的点（例如原点 0,0,0）
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

			// m_Front 代表的是看向的目标点，则直接使用
			// 这里假设你的构造函数里传的是看向的点 (0,0,0)
			m_ViewMatrix = glm::lookAt(m_Position, m_Front, up);
			m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_Aspect, m_NearPoint, m_FarPoint);
		}
	};

}