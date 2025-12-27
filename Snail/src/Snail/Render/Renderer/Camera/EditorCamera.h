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

	enum class EditorCameraMode {
		None = 0,
		FPS, Arcball
	};

	static std::string EditorCameraModeToString(const EditorCameraMode& mode)
	{
		switch (mode)
		{
			case EditorCameraMode::FPS: return "FPS";
			case EditorCameraMode::Arcball: return "Arcball";
			default: return "None";
		}
		return "";
	}

	static EditorCameraMode StringToEditorCameraMode(const std::string& modestr)
	{
		if (modestr == "FPS") { return EditorCameraMode::FPS; }
		if (modestr == "Arcball") { return EditorCameraMode::Arcball; }
		if (modestr == "None") { return EditorCameraMode::None; }
		SNL_CORE_ASSERT(false, "EditorCamera::StringToEditorCameraMode: 未知编辑器相机模式! {0}", modestr.c_str());

		return EditorCameraMode::None;
	}

	enum class EditorCameraTranslateDirection { // 平移方向
		None = 0,
		UP, LEFT, DOWN, RIGHT, FRONT, BACK
	};

	class EditorCamera : public Camera {
	public:
		EditorCamera();
		EditorCamera(const EditorCameraMode& mode);
		EditorCamera(const glm::vec3& position, const EditorCameraMode& mode);
		~EditorCamera() = default;

		void RecalculateVectors();
		void RecalculateViewMatrix();
		void RecalculateProjectionMatrix();

		void OnUpdate(const Timestep& ts);
		void OnEvent(Event& e);
	
		// --- Get 方法 ---
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4 GetTransform() const { return glm::inverse(m_ViewMatrix); }
		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::vec3 GetRotation() const { return glm::vec3(m_Pitch, m_Yaw, 0.0f); }
		const EditorCameraMode& GetMode() const { return m_Mode; }
		void SetMode(const EditorCameraMode& mode) { m_Mode = mode; }

		const float& GetFOV() const					{ return m_FOV; }
		const float& GetNear() const				{ return m_Near; }
		const float& GetFar() const					{ return m_Far; }
		const float& GetAspect() const				{ return m_Aspect; }
		const float& GetViewportWidth() const		{ return m_ViewportWidth; }
		const float& GetViewportHeight() const		{ return m_ViewportHeight; }

		const glm::vec3& GetFront() const			{ return m_Front; }
		const glm::vec3& GetRight() const			{ return m_Right; }
		const glm::vec3& GetUp() const				{ return m_Up; }
		const glm::vec3& GetWorldUp() const			{ return m_WorldUp; }
		const float& GetPitch() const				{ return m_Pitch; }
		const float& GetYaw() const					{ return m_Yaw; }

		const glm::vec3 GetFocalPoint() const		{ return m_FocalPoint; }
		const float& GetDistance() const			{ return m_Distance; }
		const float& GetRotateSensitivity() const	{ return m_RotateSensitivity; }
		const float& GetMoveSensitivity() const		{ return m_MoveSensitivity; }
		const float& GetRotateSpeed() const			{ return m_RotateSpeed; }
		const float& GetMoveSpeed() const			{ return m_MoveSpeed; }
		const float& GetDollySpeed() const			{ return m_DollySpeed; }

		// --- Set 方法 ---
		void SetPitch(const float& pitch)				{ m_Pitch = pitch; }
		void SetYaw(const float& yaw)					{ m_Yaw = yaw; }		
		void SetPosition(const glm::vec3& pos)			{ m_Position = pos; }
		void SetDistance(const float& dis)				{ m_Distance = dis; }
		void SetFocalPoint(const glm::vec3& point)		{ m_FocalPoint = point; }

		void SetViewportSize(const float& width, const float& height);
	private:
		void FPSMove(const EditorCameraTranslateDirection& dir, const float& length);
		void FPSRotate(const float& yaw, const float& pitch);
		void FPSZoom(const float& fovOffset);
		void ArcballMove(const float& xoffset, const float& yoffset);
		void ArcballRotate(const float& xoffset, const float& yoffset);
		void ArcballDolly(const float& offset);
		bool OnMouseScroll(MouseScrollEvent& e);
		// --- Arcball 辅助函数 ---
		glm::vec3 CalculatePosition() const;
		glm::vec3 GetForwardDirection() const;	
		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::quat GetOrientation() const;
		std::pair<float, float> PanSpeed() const;
		float DollySpeed() const;
	private:
		EditorCameraMode m_Mode = EditorCameraMode::FPS;

		// 投影属性
		float m_FOV = 45.0f;
		float m_Near = 1.0f;
		float m_Far = 10000.0f;
		float m_Aspect = (float)1280 / 720;
		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };

		glm::vec3 m_Front;
		glm::vec3 m_Right;
		glm::vec3 m_Up;
		glm::vec3 m_WorldUp = { 0.0f, 1.0f, 0.0f };
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f }; // 焦点（围绕旋转的点）
		float m_Distance = 10.0f;

		float m_LastMouseX;
		float m_LastMouseY;
		bool m_IsDragging = false; // 拖拽标识
		bool m_MouseButtoned = false; // 鼠标是否按下
		
		// 只有按键状态持续的情况要乘以 timestep，如键盘持续输入
		float m_RotateSensitivity = 20.0f; // 灵敏度
		float m_MoveSensitivity = 200.0f;
		// 鼠标操作的灵敏度，鼠标是每帧计算实际的屏幕偏移量，直接使用偏移量*speed即可
		float m_RotateSpeed = 0.1f;
		float m_MoveSpeed = 0.01f;
		float m_DollySpeed = 0.5f;
	};

}