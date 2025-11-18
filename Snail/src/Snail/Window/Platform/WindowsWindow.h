#pragma once

#include "Snail/Window/Window.h"

namespace Snail {

	class SNAIL_API WindowsWindow : public Window {
	private:
		GLFWwindow* m_Window;

		struct WindowData {
			WindowProps props;
			bool VSync;

			EventCallbackFn eventCallBackFn;
		};

		WindowData m_Data;
	private:
		virtual void InitWindow(const WindowProps& props);
		virtual void ShutdownWindow();
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;
		inline unsigned int GetWindowWidth() const override {
			return m_Data.props.width;
		}
		inline unsigned int GetWindowHeight() const override {
			return m_Data.props.height;
		}

		// 11.18 这三个虚函数无法理解
		inline void SetEventCallBack(const EventCallbackFn& eventCallbackFn) override {
			m_Data.eventCallBackFn = eventCallbackFn;
		}
		void SetVSync(bool enable) override;
		bool IsVSync() const override;

		static Window* SNLCreateWindow(const WindowProps& props = WindowProps());
	};

}