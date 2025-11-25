#pragma once

#include "Snail/Input/Input.h"

namespace Snail {

	class WindowsInput : public Input {
	protected:
		virtual std::pair<int, int> GetWindowSizeImpl() override;
		virtual std::pair<int, int> GetWindowPosImpl() override;
		virtual bool IsMouseButtonImpl(int button) override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
		virtual std::pair<float, float> GetMousePosImpl() override;
		virtual bool IsKeyPressedImpl(unsigned int keycode) override;
	public:
		WindowsInput() = default;
		~WindowsInput() = default;
	};

}