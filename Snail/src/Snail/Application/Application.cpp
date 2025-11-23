#include "SNLpch.h"

#include "Application.h"

#include "Snail/Input/Input.h"

namespace Snail {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		SNL_CORE_ASSERT(!s_Instance, "无法创建多个应用实例!");
		this->s_Instance = this;

		m_AppWindow = std::unique_ptr<Window>(Window::SNLCreateWindow());
		if (m_AppWindow) {
			m_Running = true;
		}
		// 将OnEvent绑定到 Window 的派生类 维护的函数指针 eventCallbackFn 之上
		m_AppWindow->SetEventCallback(BIND_NSTATIC_MEMBER_Fn(Application::OnEvent));
	}

	Application::~Application()
	{
	}

	Application& Application::Get()
	{
		return *Application::s_Instance;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		// 接收一个bool(T&),T是某个事件类型
		dispatcher.Dispatch<WindowCloseEvent>(BIND_NSTATIC_MEMBER_Fn(Application::OnWindowClose));

		SNL_CORE_INFO(e.ToString());

		// 层栈的事件处理，由顶层至底层
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++) {
			(*it)->OnEvent(e);
			if (e.GetIsHandled()) { break; }
		}
	}

	void Application::OnUpdate()
	{
		// 层栈的渲染处理，由底层至顶层
		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate();
		}
	}

	// push/pop normal layer
	void Application::PushNorLayer(Layer* norLayer) {
		m_LayerStack.PushNorLayer(norLayer);
		norLayer->OnAttach();
	}
	void Application::PopNorLayer(Layer* norLayer) {
		m_LayerStack.PopNorLayer(norLayer);
	}
	// push/pop Top layer
	void Application::PushOverLayer(Layer* overLayer) {
		m_LayerStack.PushOverLayer(overLayer);
		overLayer->OnAttach();
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
		while (m_Running) {

			m_AppWindow->OnUpdate();

			this->OnUpdate();

			std::pair<float, float> pos = Input::GetMousePos();
			SNL_CORE_TRACE("测试Input类: x-{0}, y-{1}", pos.first, pos.second);
		}
	}

}