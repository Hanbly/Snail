#pragma once

#include "Snail/Core/Core.h"

#include "Snail/Input/Input.h"

#include "Snail/Events/Event.h"
#include "Snail/Events/MouseEvent.h"

#include "Camera.h"

namespace Snail {

	class PerspectiveCameraController {
	public:
		PerspectiveCameraController(const float& fov, const float& aspect, const glm::vec3& position);
		~PerspectiveCameraController() = default;

		void MoveCamera(const Camera::TranslationDirection& dir, const float& length);
		void RotateCamera(const float& yaw, const float& pitch);

		void RecalculateMatrix();
		void RecalculateVectors();

		void OnUpdate(const Snail::Timestep& ts);
		void OnEvent(Event& e);
		bool OnMouseMoveEvent(MouseMoveEvent& e);
	public:
		inline const std::unique_ptr<Snail::Camera>& GetCamera() const { return m_Camera; }
	private:
		std::unique_ptr<Snail::Camera> m_Camera;
		float m_CameraMoveSpeed = 5.0f;

		float m_LastMouseX;
		float m_LastMouseY;
		bool  m_FirstMouse = true; // 第一帧标志位
		float m_Sensitivity = 0.1f; // 灵敏度
	};

}