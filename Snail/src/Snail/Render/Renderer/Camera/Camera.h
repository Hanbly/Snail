#pragma once

#include "Snail/Core.h"

#include "Snail/Input/Input.h"

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
		glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;

	public:

		enum class TranslationDirection { // 平移方向
			None = 0,
			UP, LEFT, DOWN, RIGHT, FRONT, BACK
		};

		Camera(const glm::float32& fov, const glm::float32& aspect, const glm::vec3& position,
			const glm::float32& np = 0.1f, const glm::float32& fp = 100.0f);
		~Camera() = default;

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const glm::vec3& GetFront() const { return m_Front; }

		void MoveCamera(const TranslationDirection& dir, const float& length);
		void SetFront(const glm::vec3& deltaFront);

		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline void SetViewMatrix(const glm::mat4& mat4) { m_ViewMatrix = mat4; }
		inline void SetProjectionMatrix(const glm::mat4& mat4) { m_ProjectionMatrix = mat4; }

		void RecalculateMatrix();
	};

}