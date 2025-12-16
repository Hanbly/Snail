#include "SNLpch.h"

#include "Snail/Basic/InputCodes.h"

#include "EditorCamera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

namespace Snail {

	EditorCamera::EditorCamera()
	{
		RecalculateVectors();
		RecalculateViewMatrix();
		RecalculateProjectionMatrix();
	}

	EditorCamera::EditorCamera(const EditorCameraMode& mode)
		: m_Mode(mode)
	{
		RecalculateVectors();
		RecalculateViewMatrix();
		RecalculateProjectionMatrix();
	}

	EditorCamera::EditorCamera(const glm::vec3& position, const EditorCameraMode& mode)
		: m_Position(position), m_Mode(mode)
	{
		RecalculateVectors();
		RecalculateViewMatrix();
		RecalculateProjectionMatrix();
	}

	void EditorCamera::RecalculateVectors() {
		if (m_Mode == EditorCameraMode::FPS) {
			glm::vec3 front;
			front.x = cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
			front.y = sin(glm::radians(m_Pitch));
			front.z = cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));
			m_Front = glm::normalize(front);
		}
		else if (m_Mode == EditorCameraMode::Arcball) {
			// 核心逻辑依赖 GetOrientation()
			m_Front = GetForwardDirection();
			m_Right = GetRightDirection();
			m_Up = GetUpDirection();
		}
		// 重新计算 Right 和 Up
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}

	void EditorCamera::RecalculateViewMatrix()
	{
		if (m_Mode == EditorCameraMode::FPS) {
			m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
		}
		else if (m_Mode == EditorCameraMode::Arcball) {
			// 相机位置 = 焦点位置 - (方向 * 距离)
			m_Position = CalculatePosition();

			glm::quat orientation = GetOrientation();
			m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
			m_ViewMatrix = glm::inverse(m_ViewMatrix);
		}
	}

	void EditorCamera::RecalculateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_Aspect, m_Near, m_Far);
	}

	// 处理 Resize
	void EditorCamera::SetViewportSize(const float& width, const float& height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
		m_Aspect = width / height;
		
		RecalculateProjectionMatrix();
	}

	void EditorCamera::OnUpdate(const Timestep& ts)
	{
		m_MouseButtoned =	Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT) ||
							Input::IsMouseButton(SNL_MOUSE_BUTTON_MIDDLE) ||
							Input::IsMouseButton(SNL_MOUSE_BUTTON_RIGHT);

		float currentMouseX = Input::GetMouseX();
		float currentMouseY = Input::GetMouseY();

		// 初始化偏移量为 0
		float xoffset = 0.0f;
		float yoffset = 0.0f;

		if (m_MouseButtoned) {
			if (!m_IsDragging) {
				// --- 拖拽的第一帧 ---
				m_IsDragging = true;
				m_LastMouseX = currentMouseX;
				m_LastMouseY = currentMouseY;
			}
			else {
				// --- 拖拽的后续帧 ---
				xoffset = currentMouseX - m_LastMouseX;
				yoffset = m_LastMouseY - currentMouseY; // 反写适应OpenGL

				m_LastMouseX = currentMouseX;
				m_LastMouseY = currentMouseY;
			}
		}
		else {
			m_IsDragging = false;
		}

		// 3. 应用视角移动 (仅当有实际位移时)
		if (xoffset != 0.0f || yoffset != 0.0f) {
			SNL_CORE_WARN("offset: {0},{1}", xoffset, yoffset);
		}

		if (m_Mode == EditorCameraMode::FPS) {
			// 移动
			if (Input::IsKeyPressed(SNL_KEY_W)) {
				this->FPSMove(EditorCameraTranslateDirection::FRONT, m_MoveSensitivity * ts);
			}
			else if (Input::IsKeyPressed(SNL_KEY_S)) {
				this->FPSMove(EditorCameraTranslateDirection::BACK, m_MoveSensitivity * ts);
			}
			if (Input::IsKeyPressed(SNL_KEY_E)) {
				this->FPSMove(EditorCameraTranslateDirection::UP, m_MoveSensitivity * ts);
			}
			else if (Input::IsKeyPressed(SNL_KEY_Q)) {
				this->FPSMove(EditorCameraTranslateDirection::DOWN, m_MoveSensitivity * ts);
			}
			if (Input::IsKeyPressed(SNL_KEY_A)) {
				this->FPSMove(EditorCameraTranslateDirection::LEFT, m_MoveSensitivity * ts);
			}
			else if (Input::IsKeyPressed(SNL_KEY_D)) {
				this->FPSMove(EditorCameraTranslateDirection::RIGHT, m_MoveSensitivity * ts);
			}
			// 视角旋转
			if (Input::IsMouseButton(SNL_MOUSE_BUTTON_MIDDLE)) {
				xoffset *= m_RotateSpeed;
				yoffset *= m_RotateSpeed;
				this->FPSRotate(xoffset, yoffset);
			}
		}
		else if (m_Mode == EditorCameraMode::Arcball) {
			// Alt + Middle Mouse = Pan (平移)
			if (Input::IsKeyPressed(SNL_KEY_SPACE) && Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT)) {
				xoffset *= m_MoveSpeed;
				yoffset *= m_MoveSpeed;

				ArcballMove(xoffset, yoffset);
			}
			// Left Mouse = Rotate (轨道旋转)
			else if (Input::IsMouseButton(SNL_MOUSE_BUTTON_LEFT)) {
				xoffset *= m_RotateSpeed;
				yoffset *= m_RotateSpeed;

				ArcballRotate(xoffset, yoffset);
			}
		}
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrollEvent>(BIND_NSTATIC_MEMBER_Fn(EditorCamera::OnMouseScroll));
	}

	void EditorCamera::FPSMove(const EditorCameraTranslateDirection& dir, const float& length)
	{
		glm::vec3 vector = glm::vec3(0.0f);
		if (dir == EditorCameraTranslateDirection::FRONT) { vector = m_Front * length; }
		else if (dir == EditorCameraTranslateDirection::BACK) { vector = -m_Front * length; }
		if (dir == EditorCameraTranslateDirection::RIGHT) { vector = m_Right * length; }
		else if (dir == EditorCameraTranslateDirection::LEFT) { vector = -m_Right * length; }
		if (dir == EditorCameraTranslateDirection::UP) { vector = m_WorldUp * length; }
		else if (dir == EditorCameraTranslateDirection::DOWN) { vector = -m_WorldUp * length; }

		m_Position = m_Position + vector;
		m_FocalPoint += vector;

		RecalculateViewMatrix();
	}

	void EditorCamera::FPSRotate(const float& yaw, const float& pitch)
	{
		m_Yaw += yaw;
		m_Pitch += pitch;

		if (m_Pitch > 89.0f) m_Pitch = 89.0f;
		if (m_Pitch < -89.0f) m_Pitch = -89.0f;

		RecalculateVectors();
		RecalculateViewMatrix();
	}

	void EditorCamera::FPSZoom(const float& fovOffset)
	{
		m_FOV -= fovOffset;

		// 防止FOV过小或过大
		if (m_FOV < 1.0f) m_FOV = 1.0f;
		if (m_FOV > 90.0f) m_FOV = 90.0f;

		RecalculateProjectionMatrix();
	}

	void EditorCamera::ArcballMove(const float& xoffset, const float& yoffset)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * xoffset * xSpeed * m_Distance;
		m_FocalPoint += -GetUpDirection() * yoffset * ySpeed * m_Distance;

		RecalculateViewMatrix();
	}

	void EditorCamera::ArcballRotate(const float& xoffset, const float& yoffset)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * xoffset * 0.005f * 180.0f;
		m_Pitch += -yoffset * 0.005f * 180.0f;

		RecalculateVectors();
		RecalculateViewMatrix();
	}

	void EditorCamera::ArcballZoom(const float& offset)
	{
		m_Distance -= offset * ZoomSpeed();
		if (m_Distance < 10.0f)
		{
			m_Distance = 10.0f;
		}

		RecalculateViewMatrix();
	}

	bool EditorCamera::OnMouseScroll(MouseScrollEvent& e)
	{
		if (m_Mode == EditorCameraMode::FPS) {
			const float& fovOffset = e.GetMouseScrollOffsetY() * 3;
			FPSZoom(fovOffset);
			return false;
		}
		else if (m_Mode == EditorCameraMode::Arcball) {
			const float& offset = e.GetMouseScrollOffsetY() * 3;
			ArcballZoom(offset);
			return false;
		}
		return false;
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-glm::radians(m_Pitch), -glm::radians(m_Yaw), 0.0f));
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // 基础速度因子
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f);
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 50.0f); // 限制最大速度
		return speed * m_ZoomSpeed;
	}

}