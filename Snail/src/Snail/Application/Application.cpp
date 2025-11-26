#include "SNLpch.h"

#include "Snail/Input/Input.h"

#include "Application.h"

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

		// -------------------临时------------------------------------------
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		float vertices[3 * 3] = {
			0.1f, 0.3f, 0.0f,
			-0.8f, -0.6f, 0.0f,
			0.5f, -0.8f, 0.0f
		};
		m_VertexBuffer = VertexBuffer::CreateVertexBuffer(vertices, sizeof(vertices));
		m_VertexBuffer->Bind();

		// 顶点缓冲区布局layout
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

		uint32_t indices[1 * 3] = {
			0, 2, 1
		};
		m_IndexBuffer = IndexBuffer::CreateIndexBuffer(indices, sizeof(indices));
		m_IndexBuffer->Bind();

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}

		)";
		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
			}

		)";
		m_Shader = Shader::CreateShader(vertexSrc, fragmentSrc);
		//------------------------------------------------------------------------------
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
		// 层栈的逻辑更新处理，由底层至顶层
		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate();
		}


		// -------------------临时------------------------------------------
		m_Shader->Bind();
		glBindVertexArray(m_VertexArray);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
		//----------------------------------------------------------------



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

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // 深灰色背景
			glClear(GL_COLOR_BUFFER_BIT);

			this->OnUpdate();
		}

	}

}