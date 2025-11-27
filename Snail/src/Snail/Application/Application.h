#pragma once

#include "Snail/Core.h"

#include "Snail/Window/Window.h"
#include "Platform/glfw/OpenGL/ImGui/ImGuiLayer/ImGuiLayer.h"
#include "Snail/Layer/LayerStack.h"
#include "Snail/Events/ApplicationEvent.h"

#include "Snail/Render/RenderAPI/Buffer/VertexBuffer.h"
#include "Snail/Render/RenderAPI/Buffer/IndexBuffer.h"
#include "Snail/Render/RenderAPI/Shader.h"

namespace Snail {

	class SNAIL_API Application
	{
	private:
		static Application* s_Instance;

		std::unique_ptr<Window> m_AppWindow;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		bool m_Running = false;

		// -------------------临时------------------------------------------
		unsigned int m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<Shader> m_Shader;
	public:
		Application();
		// Application 类会被Example所继承，所以使用virtual
		virtual ~Application();

		static Application& Get();

		void OnEvent(Event& e);
		void OnUpdate();
		// push/pop normal layer
		void PushNorLayer(Layer* norLayer);
		void PopNorLayer(Layer* norLayer);
		// push/pop Top layer
		void PushOverLayer(Layer* overLayer);
		void PopOverLayer(Layer* overLayer);

		inline Window& GetWindow() const {
			return *m_AppWindow;
		}

		void run();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	};

	// 将在客户端（子类中）给出函数体
	Application* CreateApplication();

}

