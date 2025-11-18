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
	}

	Application::~Application()
	{
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