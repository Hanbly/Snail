#pragma once

#include "Snail/Core.h"

namespace Snail {

	class SNAIL_API Input {
	private:
		static Input* s_Instance;
	protected:
		virtual std::pair<int, int> GetWindowSizeImpl() = 0;
		virtual std::pair<int, int> GetWindowPosImpl() = 0;
		virtual bool IsMouseButtonImpl(int button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual std::pair<float, float> GetMousePosImpl() = 0;
		virtual bool IsKeyPressedImpl(unsigned int keycode) = 0;
	public:
		inline static std::pair<int, int> GetWindowSize() {
			return s_Instance->GetWindowSizeImpl();
		}
		inline static std::pair<int, int> GetWindowPos() {
			return s_Instance->GetWindowPosImpl();
		}
		inline static bool IsMouseButton(int button) {
			return s_Instance->IsMouseButtonImpl(button);
		}
		inline static float GetMouseX() {
			return s_Instance->GetMouseXImpl();
		}
		inline static float GetMouseY() {
			return s_Instance->GetMouseYImpl();
		}
		inline static std::pair<float, float> GetMousePos() {
			return s_Instance->GetMousePosImpl();
		}
		inline static bool IsKeyPressed(unsigned int keycode) {
			return s_Instance->IsKeyPressedImpl(keycode);
		}
	};

}