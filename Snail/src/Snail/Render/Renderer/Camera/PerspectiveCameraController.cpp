#include "SNLpch.h"

#include "PerspectiveCameraController.h"

namespace Snail {

	PerspectiveCameraController::PerspectiveCameraController(const float& fov, const float& aspect, const glm::vec3& position)
	{
		m_Camera = std::make_unique<Camera>(fov, aspect, position);

		this->RecalculateVectors();
		this->RecalculateMatrix();
	}

	void PerspectiveCameraController::MoveCamera(const Camera::TranslationDirection& dir, const float& length) {

		glm::vec3 vector = glm::vec3(0.0f);
		if (dir == Camera::TranslationDirection::FRONT)			{ vector = m_Camera->m_Front * length; }
		else if (dir == Camera::TranslationDirection::BACK)		{ vector = -m_Camera->m_Front * length; }
		if (dir == Camera::TranslationDirection::RIGHT)			{ vector = m_Camera->m_Right * length; }
		else if (dir == Camera::TranslationDirection::LEFT)		{ vector = -m_Camera->m_Right * length; }
		if (dir == Camera::TranslationDirection::UP)			{ vector = m_Camera->m_WorldUp * length; }
		else if (dir == Camera::TranslationDirection::DOWN)		{ vector = -m_Camera->m_WorldUp * length; }

		m_Camera->m_Position = m_Camera->m_Position + vector;
		this->RecalculateMatrix();
	}

	void PerspectiveCameraController::RotateCamera(const float& yaw, const float& pitch)
	{
		// 1. 累加偏移量到总角度
		m_Camera->m_Yaw += yaw;
		m_Camera->m_Pitch += pitch;

		if (m_Camera->m_Pitch > 89.0f) m_Camera->m_Pitch = 89.0f;
		if (m_Camera->m_Pitch < -89.0f) m_Camera->m_Pitch = -89.0f;

		this->RecalculateVectors();
		this->RecalculateMatrix();
	}

	// 处理 滚轮缩放
	void PerspectiveCameraController::UpdateZoomFov(const float& fovOffset)
	{
		m_Camera->m_FOV -= fovOffset;

		// 防止FOV过小或过大
		if (m_Camera->m_FOV < 1.0f) m_Camera->m_FOV = 1.0f;
		if (m_Camera->m_FOV > 90.0f) m_Camera->m_FOV = 90.0f;

		this->RecalculateMatrix();
	}
	// 处理 Resize
	void PerspectiveCameraController::UpdateAspect(const float& aspect)
	{
		m_Camera->m_Aspect = aspect;

		this->RecalculateMatrix();
	}

	void PerspectiveCameraController::RecalculateMatrix() {
		// 根据现有相机属性，对其矩阵进行重新计算

		// 参数: 眼睛位置, 目标位置, 上向量
		// -----------------------------------------------------------
		m_Camera->m_ViewMatrix = glm::lookAt(m_Camera->m_Position, m_Camera->m_Front + m_Camera->m_Position, m_Camera->m_Up);
		m_Camera->m_ProjectionMatrix = glm::perspective(glm::radians(m_Camera->m_FOV), m_Camera->m_Aspect, m_Camera->m_Near, m_Camera->m_Far);
	}

	void PerspectiveCameraController::RecalculateVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(m_Camera->m_Pitch)) * cos(glm::radians(m_Camera->m_Yaw));
		front.y = sin(glm::radians(m_Camera->m_Pitch));
		front.z = cos(glm::radians(m_Camera->m_Pitch)) * sin(glm::radians(m_Camera->m_Yaw));
		m_Camera->m_Front = glm::normalize(front);

		// 重新计算 Right 和 Up
		m_Camera->m_Right = glm::normalize(glm::cross(m_Camera->m_Front, m_Camera->m_WorldUp));
		m_Camera->m_Up = glm::normalize(glm::cross(m_Camera->m_Right, m_Camera->m_Front));
	}

	void PerspectiveCameraController::OnUpdate(const Timestep& ts) {

		if (Input::IsKeyPressed(SNL_KEY_W)) {
			this->MoveCamera(Camera::TranslationDirection::FRONT, m_CameraMoveSpeed * ts);
		}
		else if (Input::IsKeyPressed(SNL_KEY_S)) {
			this->MoveCamera(Camera::TranslationDirection::BACK, m_CameraMoveSpeed * ts);
		}
		if (Input::IsKeyPressed(SNL_KEY_E)) {
			this->MoveCamera(Camera::TranslationDirection::UP, m_CameraMoveSpeed * ts);
		}
		else if (Input::IsKeyPressed(SNL_KEY_Q)) {
			this->MoveCamera(Camera::TranslationDirection::DOWN, m_CameraMoveSpeed * ts);
		}
		if (Input::IsKeyPressed(SNL_KEY_A)) {
			this->MoveCamera(Camera::TranslationDirection::LEFT, m_CameraMoveSpeed * ts);
		}
		else if (Input::IsKeyPressed(SNL_KEY_D)) {
			this->MoveCamera(Camera::TranslationDirection::RIGHT, m_CameraMoveSpeed * ts);
		}

	}

	void PerspectiveCameraController::OnEvent(Event& e) {

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_NSTATIC_MEMBER_Fn(PerspectiveCameraController::OnWindowResize));
		dispatcher.Dispatch<MouseMoveEvent>(BIND_NSTATIC_MEMBER_Fn(PerspectiveCameraController::OnMouseMove));
		dispatcher.Dispatch<MouseScrollEvent>(BIND_NSTATIC_MEMBER_Fn(PerspectiveCameraController::OnMouseScroll));

	}

	bool PerspectiveCameraController::OnWindowResize(WindowResizeEvent& e)
	{
		UpdateAspect((float)e.GetWindowWidth() / (float)e.GetWindowHeight());
		return false;
	}

	bool PerspectiveCameraController::OnMouseMove(MouseMoveEvent& e) {

		if (m_FirstMouse) {
			m_LastMouseX = e.GetMouseX();
			m_LastMouseY = e.GetMouseY();
			m_FirstMouse = false;
			return false;
		}
		float xoffset = e.GetMouseX() - m_LastMouseX;
		float yoffset = m_LastMouseY - e.GetMouseY();
		m_LastMouseX = e.GetMouseX();
		m_LastMouseY = e.GetMouseY();

		xoffset *= m_Sensitivity;
		yoffset *= m_Sensitivity;

		if (Input::IsMouseButton(SNL_MOUSE_BUTTON_MIDDLE)) {
			this->RotateCamera(xoffset, yoffset);
		}
		return false;

	}

	bool PerspectiveCameraController::OnMouseScroll(MouseScrollEvent& e)
	{
		const float& fovOffset = e.GetMouseScrollOffsetY() * 2;
		UpdateZoomFov(fovOffset);
		return false;
	}

}