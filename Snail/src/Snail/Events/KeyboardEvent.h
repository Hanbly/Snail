#pragma once

namespace Snail {

	// ----------------------键盘事件------------------------------####################################################
	class SNAIL_API KeyboardEvent : public Event {
	protected:
		int m_KeyCode;
		KeyboardEvent(int keycode)
			: m_KeyCode(keycode) {
		}

	public:
		inline int GetKeyCode() const { return m_KeyCode; }

		EVENT_CATEGORY_FUNC_SET(InputCategoryEvent | KeyboardCategoryEvent)
		// 这里没有实现 EVENT_TYPE_FUNC_SET 中的一些纯虚函数，也就是 KeyboardEvent 不会被/不能被实例化，是一个抽象类！
	};
	// ----------------------键盘输入事件------------------------------####################################################
	class SNAIL_API KeyPressEvent : public KeyboardEvent {
	private:
		float m_RepeatCount;
	public:
		KeyPressEvent(int keycode, float repeatcount)
			: KeyboardEvent(keycode), m_RepeatCount(repeatcount) {
		}

		inline float GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override {
			return fmt::format("[键盘输入事件]: {}, 重复次数: {}", m_KeyCode, m_RepeatCount);
		}

		EVENT_TYPE_FUNC_SET(KeyboardPress)
	};
	// ----------------------键盘释放事件------------------------------####################################################
	class SNAIL_API KeyReleaseEvent : public KeyboardEvent {
	public:
		KeyReleaseEvent(int keycode)
			: KeyboardEvent(keycode) {
		}

		std::string ToString() const override {
			return fmt::format("[键盘释放事件]: {}", m_KeyCode);
		}

		EVENT_TYPE_FUNC_SET(KeyboardRelease)
	};

}
