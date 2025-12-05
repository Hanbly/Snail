#pragma once

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

namespace Snail {

	class SNAIL_API Window {
	protected:

		using EventCallbackFn = std::function<void(Event&)>;

		struct WindowProps {
			std::string title;
			unsigned int width;
			unsigned int height;

			WindowProps(const std::string& title = "Snail Engine",
				unsigned int width = 1920,
				unsigned int height = 1080)
				: title(title), width(width), height(height) {
			}
		};
		struct WindowData {
			WindowProps props;
			bool VSync = true;

			EventCallbackFn eventCallbackFn;
		};
	public:
		Window() = default;
		virtual ~Window() = default;

		virtual void OnUpdate() = 0;
		virtual unsigned int GetWindowWidth() const = 0;
		virtual unsigned int GetWindowHeight() const = 0;
		virtual void* GetWindow() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& eventCallbackFn) = 0;
		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;
		
		static Uniptr<Window> Create(const WindowProps& props = WindowProps());
	};

}