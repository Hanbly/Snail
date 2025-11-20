#include "SNLpch.h"

#include "Snail/Events/ApplicationEvent.h"
#include "Snail/Window/Window.h"

#include "Application.h"

namespace Snail {

	Application::Application()
	{
		m_AppWindow = std::unique_ptr<Window>(Window::SNLCreateWindow());
		if (m_AppWindow) {
			m_Running = true;
		}
		// 将OnEvent绑定到 Window 的派生类 维护的函数指针 eventCallbackFn 之上
		m_AppWindow->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		// 接收一个bool(T&),T是某个事件类型
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

		SNL_CORE_INFO(e.ToString());
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	void Application::run()
	{
		WindowResizeEvent winRE = WindowResizeEvent(1000, 750);
		SNL_TRACE(winRE.ToString());

		//TODO 某些处理事件的函数？函数符合 bool(事件类型&)
		/*EventDispatcher ed = EventDispatcher(winRE);
		ed.Dispatch()*/

		while (m_Running) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_AppWindow->OnUpdate();
		}
	}

}