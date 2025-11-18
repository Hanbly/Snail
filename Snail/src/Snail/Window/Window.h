#pragma once

namespace Snail {

	struct WindowProps {
		std::string title;
		unsigned int width;
		unsigned int height;

		WindowProps(const std::string& title = "Snail Engine",
					unsigned int width = 1280,
					unsigned int height = 720)
			: title(title), width(width), height(height) {}
	};

	class SNAIL_API Window {
	public:
		Window() = default;
		virtual ~Window() {}

		using EventCallbackFn = std::function<void(Event&)>;

		virtual void OnUpdate() = 0;
		virtual unsigned int GetWindowWidth() const = 0;
		virtual unsigned int GetWindowHeight() const = 0;

		// 11.18 这三个虚函数无法理解
		virtual void SetEventCallBack(const EventCallbackFn& eventCallbackFn) = 0;
		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;
		
		static Window* SNLCreateWindow(const WindowProps& props = WindowProps());
	};

}