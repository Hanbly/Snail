#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Input/Input.h"
#include "Snail/Basic/Timestep.h"

#include "Snail/Events/Event.h"
#include "Snail/Events/ApplicationEvent.h"
#include "Snail/Events/MouseEvent.h"

#include "Camera.h"

namespace Snail {

	class PerspectiveCameraController {
	public:
		PerspectiveCameraController(const float& fov, const float& aspect, const glm::vec3& position);
		~PerspectiveCameraController() = default;

		void RecalculateMatrix();
		void RecalculateVectors();

		void MoveCamera(const Camera::TranslationDirection& dir, const float& length);
		void RotateCamera(const float& yaw, const float& pitch);
		void UpdateZoomFov(const float& fovOffset);
		void UpdateAspect(const float& aspect);

		void OnUpdate(const Timestep& ts);
		void OnEvent(Event& e);
	
		inline const Uniptr<Camera>& GetCamera() const { return m_Camera; }
	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnMouseMove(MouseMoveEvent& e);
		bool OnMouseScroll(MouseScrollEvent& e);
	private:
		Uniptr<Camera> m_Camera;
		float m_CameraMoveSpeed = 150.0f;

		float m_LastMouseX;
		float m_LastMouseY;
		bool  m_FirstMouse = true; // 第一帧标志位
		float m_Sensitivity = 0.1f; // 灵敏度
	};

}