#pragma once

#include "Event.h"

namespace Snail {

	// ----------------------窗口缩放事件------------------------------####################################################
	class SNAIL_API WindowResizeEvent : public Event {
	private:
		unsigned int m_Width;
		unsigned int m_Height;
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {
		}

		inline unsigned int GetWindowWidth() const { return m_Width; }
		inline unsigned int GetWindowHeight() const { return m_Height; }

		std::string ToString() const override {
			return fmt::format("[窗口缩放事件] 当前窗口大小尺寸: 宽-{}|长-{}", m_Width, m_Height);
		}

		EVENT_TYPE_FUNC_SET(WindowResize)
		EVENT_CATEGORY_FUNC_SET(ApplicationCategoryEvent)
	};
	// ----------------------窗口移动事件------------------------------####################################################
	class SNAIL_API WindowMoveEvent : public Event {
	private:
		unsigned int m_X;
		unsigned int m_Y;
	public:
		WindowMoveEvent(unsigned int x, unsigned int y)
			: m_X(x), m_Y(y) {
		}

		inline unsigned int GetWindowX() const { return m_X; }
		inline unsigned int GetWindowY() const { return m_Y; }

		std::string ToString() const override {
			return fmt::format("[窗口移动事件] 当前窗口位置: X-{}|Y-{}", m_X, m_Y);
		}

		EVENT_TYPE_FUNC_SET(WindowMove)
		EVENT_CATEGORY_FUNC_SET(ApplicationCategoryEvent)
	};
	// ----------------------窗口关闭事件------------------------------####################################################
	class SNAIL_API WindowCloseEvent : public Event {
	public:
		WindowCloseEvent() {}

		std::string ToString() const override {
			return fmt::format("[窗口关闭事件]");
		}

		EVENT_TYPE_FUNC_SET(WindowClose)
		EVENT_CATEGORY_FUNC_SET(ApplicationCategoryEvent)
	};
	// ----------------------窗口聚焦事件------------------------------####################################################
	class SNAIL_API WindowFocusEvent : public Event {
	public:
		WindowFocusEvent() {}

		std::string ToString() const override {
			return fmt::format("[窗口聚焦事件]");
		}

		EVENT_TYPE_FUNC_SET(WindowFocus)
		EVENT_CATEGORY_FUNC_SET(ApplicationCategoryEvent)
	};
	// ----------------------窗口失焦事件------------------------------####################################################
	class SNAIL_API WindowLostFocusEvent : public Event {
	public:
		WindowLostFocusEvent() {}

		std::string ToString() const override {
			return fmt::format("[窗口失焦事件]");
		}

		EVENT_TYPE_FUNC_SET(WindowLostFocus)
		EVENT_CATEGORY_FUNC_SET(ApplicationCategoryEvent)
	};
	// ----------------------AppTick事件------------------------------####################################################
	class SNAIL_API AppTickEvent : public Event {
	public:
		AppTickEvent() {}

		std::string ToString() const override {
			return fmt::format("[AppTick事件]");
		}

		EVENT_TYPE_FUNC_SET(AppTick)
		EVENT_CATEGORY_FUNC_SET(ApplicationCategoryEvent)
	};
	// ----------------------AppUpdate事件------------------------------####################################################
	class SNAIL_API AppUpdateEvent : public Event {
	public:
		AppUpdateEvent() {}

		std::string ToString() const override {
			return fmt::format("[AppUpdate事件]");
		}

		EVENT_TYPE_FUNC_SET(AppUpdate)
		EVENT_CATEGORY_FUNC_SET(ApplicationCategoryEvent)
	};
	// ----------------------AppRender事件------------------------------####################################################
	class SNAIL_API AppRenderEvent : public Event {
	public:
		AppRenderEvent() {}

		std::string ToString() const override {
			return fmt::format("[AppRender事件]");
		}

		EVENT_TYPE_FUNC_SET(AppRender)
		EVENT_CATEGORY_FUNC_SET(ApplicationCategoryEvent)
	};

}