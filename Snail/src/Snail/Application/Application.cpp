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

		// 层栈的事件处理，由顶层至底层
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++) {
			(*it)->OnEvent(e);
			if (e.GetIsHandled()) { break; }
		}
	}

	// push/pop normal layer
	void Application::PushNorLayer(Layer* norLayer) {
		m_LayerStack.PushNorLayer(norLayer);
	}
	void Application::PopNorLayer(Layer* norLayer) {
		m_LayerStack.PopNorLayer(norLayer);
	}
	// push/pop Top layer
	void Application::PushOverLayer(Layer* overLayer) {
		m_LayerStack.PushOverLayer(overLayer);
	}
	void Application::PopOverLayer(Layer* overLayer) {
		m_LayerStack.PopOverLayer(overLayer);
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

			// 层栈的渲染处理，由底层至顶层
			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate();
			}
			m_AppWindow->OnUpdate();
		}
	}

}