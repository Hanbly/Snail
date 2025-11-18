#pragma once

#include "Snail/Core.h"

namespace Snail {

	enum class EventType {
		None = 0,
		AppTick, AppUpdate, AppRender,
		KeyboardPress, KeyboardRelease,
		MouseButtonPress, MouseButtonRelease, MouseMove, MouseScroll,
		WindowResize, WindowMove, WindowClose, WindowFocus, WindowLostFocus,
	};

	enum EventCategory {
		None = 0,
		ApplicationCategoryEvent = BIT(0),
		InputCategoryEvent = BIT(1),
		KeyboardCategoryEvent = BIT(2),
		MouseCategoryEvent = BIT(3),
		MouseButtonCategoryEvent = BIT(4),
	};

#define EVENT_TYPE_FUNC_SET(type) \
		static EventType GetStaticType() { return EventType::##type; }\
		virtual EventType GetEventType() const override { return GetStaticType(); }\
		virtual const char* GetEventName() const override { return #type; }

#define EVENT_CATEGORY_FUNC_SET(category) \
		virtual int GetEventCategoryFlags() const override { return category; }

	class SNAIL_API Event {
		friend class EventDispatcher;
	protected:
		bool m_Handled = false;
	public:
		virtual EventType GetEventType() const = 0;
		virtual int GetEventCategoryFlags() const = 0;
		virtual const char* GetEventName() const = 0;
		virtual std::string ToString() const { return GetEventName(); }

		inline bool IsEventInCategory(int categoryflags) const {
			return GetEventCategoryFlags() & categoryflags;
		}
	};

	class SNAIL_API EventDispatcher {
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	private:
		Event& m_Event;
	public:
		EventDispatcher(Event& event)
			: m_Event(event) {
		}

		template<typename T>
		bool Dispatch(EventFn<T> eventFunc) {
			if (m_Event.GetEventType == T::GetStaticType) {
				m_Event.m_Handled = eventFunc(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}
	};

	std::ostream& operator<<(std::ostream & os, const Event & e) {
		return os << e.ToString();
	}

}
