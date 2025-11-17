#pragma once

#include "Event.h"

namespace Snail {

	// ----------------------鼠标按钮事件抽象类------------------------------################################################
	class SNAIL_API MouseButttonEvent : public Event {
	protected:
		int m_button;
		MouseButttonEvent(int button)
			: m_button(button) {
		}
	public:
		inline int GetMouseButton() const { return m_button; }

		EVENT_CATEGORY_FUNC_SET(InputCategoryEvent | MouseButtonCategoryEvent)
	};
	// ----------------------鼠标按钮输入事件------------------------------####################################################
	class SNAIL_API MousePressEvent : public MouseButttonEvent {
	public:
		MousePressEvent(int button)
			: MouseButttonEvent(button) {
		}

		std::string ToString() const override {
			return fmt::format("[鼠标按钮输入事件]: {}", m_button);
		}

		EVENT_TYPE_FUNC_SET(MouseButtonPress)
	};
	// ----------------------鼠标按钮释放事件------------------------------####################################################
	class SNAIL_API MouseReleaseEvent : public MouseButttonEvent {
	public:
		MouseReleaseEvent(int button)
			: MouseButttonEvent(button) {
		}

		std::string ToString() const override {
			return fmt::format("[鼠标按钮释放事件]: {}", m_button);
		}

		EVENT_TYPE_FUNC_SET(MouseButtonRelease)
	};

	// ----------------------鼠标移动事件------------------------------####################################################
	class SNAIL_API MouseMoveEvent : public Event {
	private:
		float m_MouseX;
		float m_MouseY;
	public:
		MouseMoveEvent(float mousex, float mousey)
			: m_MouseX(mousex), m_MouseY(mousey) {
		}

		inline float GetMouseX() const { return m_MouseX; }
		inline float GetMouseY() const { return m_MouseY; }

		std::string ToString() const override {
			return fmt::format("[鼠标移动事件] 当前鼠标位置: X-{}|Y-{}", m_MouseX, m_MouseY);
		}

		EVENT_TYPE_FUNC_SET(MouseMove)
		EVENT_CATEGORY_FUNC_SET(InputCategoryEvent | MouseCategoryEvent)
	};
	// ----------------------鼠标移动事件------------------------------####################################################
	class SNAIL_API MouseScrollEvent : public Event {
	private:
		float m_MouseScrollOffsetX;
		float m_MouseScrollOffsetY;
	public:
		MouseScrollEvent(float xoffset, float yoffset)
			: m_MouseScrollOffsetX(xoffset), m_MouseScrollOffsetY(yoffset) {
		}

		inline float GetMouseScrollOffsetX() const { return m_MouseScrollOffsetX; }
		inline float GetMouseScrollOffsetY() const { return m_MouseScrollOffsetY; }

		std::string ToString() const override {
			return fmt::format("[鼠标滚动事件] 鼠标滚动偏移量: X轴偏移量-{}|Y轴偏移量-{}", 
					m_MouseScrollOffsetX, m_MouseScrollOffsetY);
		}

		EVENT_TYPE_FUNC_SET(MouseScroll)
		EVENT_CATEGORY_FUNC_SET(InputCategoryEvent | MouseCategoryEvent)
	};

}