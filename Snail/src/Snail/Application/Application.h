#pragma once

#include "Snail/Core.h"

#include "Snail/Events/ApplicationEvent.h"
#include "Snail/Window/Window.h"

namespace Snail {

	class SNAIL_API Application
	{
	private:
		std::unique_ptr<Window> m_AppWindow;
		bool m_Running = false;
	public:
		Application();
		// Application 类会被Example所继承，所以使用virtual
		virtual ~Application();

		void OnEvent(Event& e);

		void run();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	};

	// 将在客户端（子类中）给出函数体
	Application* CreateApplication();

}

