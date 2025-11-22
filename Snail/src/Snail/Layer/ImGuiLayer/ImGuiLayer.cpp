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
		ImGui_ImplOpenGL3_Init("#version 410");

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

		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowResizeEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnWindowResize));
		dispatcher.Dispatch<WindowCloseEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnWindowClose));
		dispatcher.Dispatch<MousePressEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnMouseButtonPress));
		dispatcher.Dispatch<MouseReleaseEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnMouseButtonRelease));
		dispatcher.Dispatch<MouseMoveEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnMouseMove));
		dispatcher.Dispatch<MouseScrollEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnMouseScroll));
		dispatcher.Dispatch<KeyPressEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnKeyboardPress));
		dispatcher.Dispatch<KeyReleaseEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnKeyboardRelease));
		dispatcher.Dispatch<KeyTypeEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnKeyboardType));


	}

	bool ImGuiLayer::OnWindowResize(WindowResizeEvent& e) {
		
		ImGuiIO& io = ImGui::GetIO();
		
		io.DisplaySize = ImVec2(e.GetWindowWidth(), e.GetWindowHeight());
		// 不知道啥意思------------------------------------------
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		glViewport(0, 0, e.GetWindowWidth(), e.GetWindowHeight());
		//-----------------------------------------------------

		return false;
	}
	bool ImGuiLayer::OnWindowClose(WindowCloseEvent& e) {

		return false;
	}
	bool ImGuiLayer::OnMouseButtonPress(MousePressEvent& e) {

		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = true;

		return false;
	}
	bool ImGuiLayer::OnMouseButtonRelease(MouseReleaseEvent& e) {

		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = false;

		return false;
	}
	bool ImGuiLayer::OnMouseMove(MouseMoveEvent& e) {

		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.GetMouseX(), e.GetMouseY());

		return false;
	}
	bool ImGuiLayer::OnMouseScroll(MouseScrollEvent& e) {

		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH = e.GetMouseScrollOffsetX();
		io.MouseWheel = e.GetMouseScrollOffsetY();

		return false;
	}
	bool ImGuiLayer::OnKeyboardPress(KeyPressEvent& e) {

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		GLFWwindow* window = app.GetWindow().GetWindow();

		io.AddKeyEvent(e.GetImGuiKey(), true);

		io.AddKeyEvent(ImGuiMod_Ctrl, (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS));
		io.AddKeyEvent(ImGuiMod_Shift, (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS));
		io.AddKeyEvent(ImGuiMod_Alt, (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS));
		io.AddKeyEvent(ImGuiMod_Super, (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS));

		return false;
	}
	bool ImGuiLayer::OnKeyboardRelease(KeyReleaseEvent& e) {

        ImGuiIO& io = ImGui::GetIO();
        io.AddKeyEvent(e.GetImGuiKey(), false);

		return false;
	}
	bool ImGuiLayer::OnKeyboardType(KeyTypeEvent& e) {

		ImGuiIO& io = ImGui::GetIO();
		int keycode = e.GetKeyCode();
		io.AddInputCharacter(static_cast<unsigned int>(keycode));

		return false;
	}

}