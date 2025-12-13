#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Input/Input.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Snail {

	class Camera {
		friend class PerspectiveCameraController;
	public:

		enum class TranslationDirection { // 平移方向
			None = 0,
			UP, LEFT, DOWN, RIGHT, FRONT, BACK
		};

		Camera(const float& fov, const float& aspect);
		Camera(const float& fov, const float& aspect, const float& cnear, const float& cfar);
		~Camera() = default;

		void RecalculateProjection();
	public:

		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

	private:
		//// 相机属性
		//glm::vec3 m_Position;
		//glm::vec3 m_Front;		// 前方向量 (始终指向前方，长度为1)
		//glm::vec3 m_Up;    
		//glm::vec3 m_WorldUp;	// 世界坐标的上 (0,1,0)
		//glm::vec3 m_Right;		// 右向量

		//// 欧拉角 (累积值)
		//float m_Yaw;
		//float m_Pitch;

		// 投影属性
		float m_FOV;
		float m_Aspect;
		float m_Near = 0.1f;
		float m_Far = 2000.0f;

		// 矩阵缓存
		//glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
	};

}