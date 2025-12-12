#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

namespace Snail {

	class SNAIL_API Window {
	protected:

		using EventCallbackFn = std::function<void(Event&)>;

		struct WindowProps {
			std::string title;
			uint32_t width;
			uint32_t height;

			WindowProps(const std::string& title = "Snail Engine",
				uint32_t width = 1920,
				uint32_t height = 1080)
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
		virtual uint32_t GetWindowWidth() const = 0;
		virtual uint32_t GetWindowHeight() const = 0;
		virtual void* GetWindow() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& eventCallbackFn) = 0;
		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;
		
		static Uniptr<Window> Create(const WindowProps& props = WindowProps());
	};

}