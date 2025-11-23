#pragma once

#include "Event.h"

#include "Snail/ImGui/ImGuiLib/GlfwKey_To_ImGuiKey.h"

namespace Snail {

	// ----------------------键盘事件------------------------------####################################################
	class SNAIL_API KeyboardEvent : public Event {
	protected:
		unsigned int m_KeyCode;
		KeyboardEvent(unsigned int keycode)
			: m_KeyCode(keycode) {
		}

	public:
		inline unsigned int GetKeyCode() const { return m_KeyCode; }

		inline ImGuiKey GetImGuiKey() const { return Snail_GlfwKey_To_ImGuiKey(m_KeyCode); }

		EVENT_CATEGORY_FUNC_SET(InputCategoryEvent | KeyboardCategoryEvent)
		// 这里没有实现 EVENT_TYPE_FUNC_SET 中的一些纯虚函数，也就是 KeyboardEvent 不会被/不能被实例化，是一个抽象类！
	};
	// ----------------------键盘按下事件------------------------------####################################################
	class SNAIL_API KeyPressEvent : public KeyboardEvent {
	private:
		int m_RepeatCount;
	public:
		KeyPressEvent(unsigned int keycode, int repeatcount)
			: KeyboardEvent(keycode), m_RepeatCount(repeatcount) {
		}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override {
			return fmt::format("[键盘按下事件]: {}, 重复次数: {}", m_KeyCode, m_RepeatCount);
		}

		EVENT_TYPE_FUNC_SET(KeyboardPress)
	};
	// ----------------------键盘释放事件------------------------------####################################################
	class SNAIL_API KeyReleaseEvent : public KeyboardEvent {
	public:
		KeyReleaseEvent(unsigned int keycode)
			: KeyboardEvent(keycode) {
		}

		std::string ToString() const override {
			return fmt::format("[键盘释放事件]: {}", m_KeyCode);
		}

		EVENT_TYPE_FUNC_SET(KeyboardRelease)
	};
	// ----------------------键盘输入事件------------------------------####################################################
	class SNAIL_API KeyTypeEvent : public KeyboardEvent {
	public:
		KeyTypeEvent(unsigned int keycode)
			: KeyboardEvent(keycode) {
		}

		std::string ToString() const override {
			return fmt::format("[键盘输入事件]: {}", m_KeyCode);
		}

		EVENT_TYPE_FUNC_SET(KeyboardType)
	};

}
