#include "SNLpch.h"

#include "Application.h"

#include "Snail/Render/Renderer/RendererCommand.h"

namespace Snail {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		SNL_CORE_ASSERT(!s_Instance, "Application: 无法创建多个应用实例!");
		this->s_Instance = this;

		m_AppWindow = std::unique_ptr<Window>(Window::SNLCreateWindow());
		SNL_CORE_ASSERT(m_AppWindow, "Application: 创建窗口成员失败!");
		m_Running = true;

		// 将OnEvent绑定到 Window 的派生类 维护的函数指针 eventCallbackFn 之上
		m_AppWindow->SetEventCallback(BIND_NSTATIC_MEMBER_Fn(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		this->PushOverLayer(m_ImGuiLayer);
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

		// 层栈的事件处理，由顶层至底层
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++) {
			(*it)->OnEvent(e);
			if (e.GetIsHandled()) { break; }
		}
	}

	void Application::OnUpdate()
	{
		//-----------------计算上一帧的时间和帧间隔时间------------------
		float time = (float)glfwGetTime();
		Timestep ts = Timestep(time - m_LastFrameTime);
		m_LastFrameTime = time;

		// 层栈的逻辑更新处理，由底层至顶层
		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate(ts);
		}

		// 层栈的渲染处理，由底层至顶层
		//m_ImGuiLayer->BeginImGui();
		for (Layer* layer : m_LayerStack) {
			layer->OnRender();
		}
		//m_ImGuiLayer->EndImGui();

		// 窗口的（事件等）
		// 轮询事件 & 交换缓冲区
		m_AppWindow->OnUpdate();

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

			RendererCommand::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			RendererCommand::Clear();

			RendererCommand::EnableDepthTest();

			this->OnUpdate();
		}

	}

}