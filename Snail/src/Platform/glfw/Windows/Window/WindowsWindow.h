#pragma once

#include "Snail/Render/RenderAPI/RenderContext.h"

#include "Snail/Window/Window.h"

namespace Snail {

	class SNAIL_API WindowsWindow : public Window {
	private:
		GLFWwindow* m_Window;
		Uniptr<RenderContext> m_RenderContext;
		WindowData m_Data;
	private:
		virtual void InitWindow(const WindowProps& props);
		virtual void ShutdownWindow();
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();
		WindowsWindow(const WindowsWindow&) = delete;
		WindowsWindow& operator=(const WindowsWindow&) = delete;

		void OnUpdate() override;
		inline uint32_t GetWindowWidth() const override {
			return m_Data.props.width;
		}
		inline uint32_t GetWindowHeight() const override {
			return m_Data.props.height;
		}
		inline void* GetWindow() const override {
			return m_Window;
		}

		inline void SetEventCallback(const EventCallbackFn& eventCallbackFn) override {
			m_Data.eventCallbackFn = eventCallbackFn;
		}
		void SetVSync(bool enable) override;
		bool IsVSync() const override;
	};

}