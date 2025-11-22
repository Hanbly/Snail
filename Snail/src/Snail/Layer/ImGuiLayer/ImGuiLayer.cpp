#include "SNLpch.h"

#include "ImGuiLayer.h"

#include "Snail/ImGui/Platform/OpenGL/imgui_impl_opengl3.h"
#include "Snail/ImGui/Platform/OpenGL/imgui_impl_glfw.h"

#include "Snail/Application/Application.h"

namespace Snail {

	ImGuiLayer::ImGuiLayer(const std::string& layerName, const bool& layerEnabled)
		: Layer(layerName, layerEnabled) {}
	ImGuiLayer::~ImGuiLayer() {

	}

	void ImGuiLayer::OnAttach() {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// 初始化平台后端 (GLFW)
		Application& app = Application::Get();
		ImGui_ImplGlfw_InitForOpenGL(app.GetWindow().GetWindow(), false); // true 表示安装默认回调函数

		// 初始化渲染器后端 (OpenGL3)
		ImGui_ImplOpenGL3_Init("#version 410"); // 建议指定 GLSL 版本号

	}
	void ImGuiLayer::OnDetach() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	void ImGuiLayer::OnUpdate() {

		ImGuiIO& io = ImGui::GetIO();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		ImGui::ShowDemoWindow();

		// Rendering
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		ImGui::Render();


		Application& app = Application::Get();
		int display_w = app.GetWindow().GetWindowWidth();
		int display_h = app.GetWindow().GetWindowHeight();

		io.DisplaySize = ImVec2(display_w, display_h);

		glfwGetFramebufferSize(app.GetWindow().GetWindow(), &display_w, & display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
	void ImGuiLayer::OnEvent(Event& event) {

	}

}