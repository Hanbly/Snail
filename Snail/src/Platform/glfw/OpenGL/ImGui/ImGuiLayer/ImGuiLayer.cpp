#include "SNLpch.h"

#include "ImGuiLayer.h"

#include "Platform/glfw/OpenGL/ImGui/ImGuiLib/imgui_impl_opengl3.h"
#include "Snail/ImGui/ImGuiLib/imgui_impl_glfw.h"

#include "Snail/Application/Application.h"

namespace Snail {

	ImGuiLayer::ImGuiLayer(const std::string& layerName, const bool& layerEnabled)
		: Layer(layerName, layerEnabled) {}

	ImGuiLayer::~ImGuiLayer() 
	{
	}

	void ImGuiLayer::OnAttach() 
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		// Setup ImGui style
		//ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
		SetDarkThemeColors();

		// 初始化平台后端 (GLFW)
		Application& app = Application::Get();
		auto window = static_cast<GLFWwindow*>(app.GetWindow().GetWindow());
		ImGui_ImplGlfw_InitForOpenGL(window, true); // true 表示安装默认回调函数

		// 初始化渲染器后端 (OpenGL3)
		ImGui_ImplOpenGL3_Init("#version 410");

		ImFont* font = io.Fonts->AddFontFromFileTTF("F:/Snail/Snail/assets/fonts/cn.ttf", 22.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());

	}

	void ImGuiLayer::OnDetach() 
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnUpdate(const Timestep& ts)
	{
	}

	void ImGuiLayer::OnEvent(Event& event) 
	{
		// 当window事件发生时，会传递到imgui这里
		// 此时根据内部的阻塞状态：m_BlockMouseEvents 和 m_BlockKeyEvents 来判断
		// 如果为 true 表示阻塞/捕获事件，允许imgui层把对应的事件状态表示为 "已处理" 即SetHandled(true)
		// 如果为 false 表示禁止imgui阻塞事件，不执行任何事件状态处理，能够让其他层看到对应事件
		ImGuiIO& io = ImGui::GetIO();
		if (m_BlockMouseEvents)
		{
			// 如果是鼠标事件，且 ImGui 想要捕获鼠标，则拦截
			if (event.IsEventInCategory(EventCategory::MouseCategoryEvent) && io.WantCaptureMouse)
			{
				event.SetHandled(true);
			}
		}
		if (m_BlockKeyEvents)
		{
			// 当视口依据自身聚焦状态给出 m_BlockKeyEvents 状态为真（视口未聚焦，事件属于其它imgui窗口）
			// 只要是键盘事件，直接拦截
			// 不管 io.WantCaptureKeyboard 是真是假
			if (event.IsEventInCategory(EventCategory::KeyboardCategoryEvent))
			{
				event.SetHandled(true);
			}
		}

		/*EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowResizeEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnWindowResize));
		dispatcher.Dispatch<WindowCloseEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnWindowClose));
		dispatcher.Dispatch<MousePressEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnMouseButtonPress));
		dispatcher.Dispatch<MouseReleaseEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnMouseButtonRelease));
		dispatcher.Dispatch<MouseMoveEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnMouseMove));
		dispatcher.Dispatch<MouseScrollEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnMouseScroll));
		dispatcher.Dispatch<KeyPressEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnKeyboardPress));
		dispatcher.Dispatch<KeyReleaseEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnKeyboardRelease));
		dispatcher.Dispatch<KeyTypeEvent>(BIND_NSTATIC_MEMBER_Fn(ImGuiLayer::OnKeyboardType));*/

	}

	void ImGuiLayer::OnRender()
	{

	}

	void ImGuiLayer::BeginImGui()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		//ImGui::ShowDemoWindow();
	}

	void ImGuiLayer::EndImGui()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Rendering
		ImGui::Render();

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

	void ImGuiLayer::BeginDockingSpace()
	{
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		bool p_open = true;
		ImGui::Begin("DockSpace", &p_open, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu(u8"文件"))
			{				
				if (ImGui::MenuItem(u8"退出"))
					Application::Get().quit();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu(u8"Themes"))
			{
				if (ImGui::MenuItem(u8"深色(Dark)"))
					SetDarkThemeColors();
				if (ImGui::MenuItem(u8"浅色(Light)"))
					SetLightThemeColors();
				if (ImGui::MenuItem(u8"赛博朋克(Cyberpunk Neon)"))
					SetCyberpunkThemeColors();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	static ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs) {
		return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
	}

	static ImVec4 operator*(const ImVec4& lhs, float rhs) {
		return ImVec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;

		style.WindowBorderSize = 0.0f; // Dark 模式通常没有粗边框
		style.FrameBorderSize = 0.0f;
		style.PopupBorderSize = 0.0f; // 或者 1.0f 看你喜好

		// --- 样式调整 (增加质感的关键) ---
		style.WindowRounding = 5.3f;    // 窗口圆角
		style.FrameRounding = 2.3f;     // 输入框/按钮圆角
		style.ScrollbarRounding = 0;    // 滚动条圆角
		style.GrabRounding = 2.3f;      // 滑块圆角
		style.TabRounding = 4.0f;       // 标签页圆角
		style.PopupRounding = 4.0f;     // 弹窗圆角

		// --- 颜色定义 ---

		// 1. 窗口与背景 (基础色调：深灰)
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.105f, 0.11f, 1.00f); // 主窗口背景
		colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.125f, 1.00f);  // 子窗口背景
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);   // 弹窗背景
		colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);    // 边框 (深色描边增加立体感)
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		// 2. 文字 (高对比度)
		colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);      // 主文字
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // 禁用文字

		// 3. 标题栏 (顶部)
		colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);

		// 4. 输入框与背景 (Frame) - 比如 InputFloat, Checkbox
		// 让输入框比背景稍浅，形成凹陷或凸起感
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.055f, 0.06f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.08f, 0.085f, 0.09f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.08f, 0.085f, 0.09f, 1.00f);

		// 5. 按钮 (Button)
		colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.23f, 1.00f);        // 平常状态 (深灰色)
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.28f, 0.29f, 1.00f); // 悬停 (稍亮)
		colors[ImGuiCol_ButtonActive] = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);  // 按下 (稍暗)

		// 6. 标签页 (Tab) - 比如 Docking 的标签
		colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.22f, 0.225f, 0.235f, 1.00f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.20f, 0.205f, 0.21f, 1.00f);  // 选中状态 (稍微突出)
		colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f); // 去掉顶部的线，保持扁平
		colors[ImGuiCol_TabDimmed] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.16f, 0.165f, 0.17f, 1.00f);

		// 7. 强调色 (Header & Slider & Separator) - 这里使用经典的 ImGui 蓝，但饱和度降低一点更耐看
		ImVec4 accentColor = ImVec4(0.20f, 0.45f, 0.70f, 1.00f); // 强调色基准

		// 树形菜单选中项 / 菜单项
		colors[ImGuiCol_Header] = accentColor * ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 选中但未悬停
		colors[ImGuiCol_HeaderHovered] = accentColor * ImVec4(1.0f, 1.0f, 1.0f, 0.8f);
		colors[ImGuiCol_HeaderActive] = accentColor;

		// 分割线
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

		// 滑块与滚动条
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

		colors[ImGuiCol_SliderGrab] = accentColor * ImVec4(1.1f, 1.1f, 1.1f, 0.9f); // 比强调色稍亮
		colors[ImGuiCol_SliderGrabActive] = accentColor * ImVec4(1.3f, 1.3f, 1.3f, 1.0f);

		colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // 复选框打钩用白色，更清晰

		// Docking 预览色
		colors[ImGuiCol_DockingPreview] = accentColor * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

		// 文本选中
		colors[ImGuiCol_TextSelectedBg] = accentColor * ImVec4(1.0f, 1.0f, 1.0f, 0.35f);
		colors[ImGuiCol_NavCursor] = accentColor;
	}

	void ImGuiLayer::SetLightThemeColors()
	{
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;

		style.WindowBorderSize = 0.0f; // Light 模式通常依靠颜色区分，或者设为 1.0f
		style.FrameBorderSize = 0.0f;
		style.PopupBorderSize = 0.0f;

		style.WindowRounding = 4.0f;
		style.FrameRounding = 2.0f;
		style.GrabRounding = 2.0f;

		// 1. 背景：灰白色，不是纯白
		colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);

		// 2. 文字：深灰/黑
		colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

		// 3. 边框：浅色主题必须有边框，否则分不清区域
		colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.15f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		// 4. 输入框：纯白背景，深色边框
		colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);

		// 5. 标题栏
		colors[ImGuiCol_TitleBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f); // 保持与背景一致
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);

		// 6. 按钮
		colors[ImGuiCol_Button] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);

		// 7. 强调色：蓝紫色
		ImVec4 accent = ImVec4(0.35f, 0.45f, 0.80f, 1.00f);

		colors[ImGuiCol_Header] = accent * ImVec4(1, 1, 1, 0.3f);
		colors[ImGuiCol_HeaderHovered] = accent * ImVec4(1, 1, 1, 0.5f);
		colors[ImGuiCol_HeaderActive] = accent * ImVec4(1, 1, 1, 0.7f);

		colors[ImGuiCol_Tab] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
		colors[ImGuiCol_TabSelected] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); // 选中变白
		colors[ImGuiCol_TabSelectedOverline] = accent; // 顶部加一条彩线

		colors[ImGuiCol_SliderGrab] = accent;
		colors[ImGuiCol_SliderGrabActive] = accent * ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
		colors[ImGuiCol_CheckMark] = accent;
	}

	void ImGuiLayer::SetCyberpunkThemeColors()
	{
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;

		style.WindowRounding = 0.0f; // 赛博朋克通常是硬朗的直角
		style.FrameRounding = 0.0f;
		style.GrabRounding = 0.0f;
		style.FrameBorderSize = 1.0f; // 加上边框
		style.WindowBorderSize = 1.0f;

		ImVec4 bgDark = ImVec4(0.02f, 0.02f, 0.05f, 1.00f); // 近乎黑的深蓝
		ImVec4 bgLight = ImVec4(0.1f, 0.1f, 0.15f, 1.00f);
		ImVec4 neonPurple = ImVec4(0.80f, 0.00f, 1.00f, 1.00f);
		ImVec4 neonCyan = ImVec4(0.00f, 0.90f, 0.90f, 1.00f);

		colors[ImGuiCol_WindowBg] = bgDark;
		colors[ImGuiCol_ChildBg] = bgDark;
		colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.95f);

		colors[ImGuiCol_Border] = neonPurple * ImVec4(1, 1, 1, 0.5f); // 紫色边框
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

		// 输入框
		colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
		colors[ImGuiCol_FrameBgHovered] = neonCyan * ImVec4(1, 1, 1, 0.2f);
		colors[ImGuiCol_FrameBgActive] = neonCyan * ImVec4(1, 1, 1, 0.4f);

		// 标题栏
		colors[ImGuiCol_TitleBg] = bgLight;
		colors[ImGuiCol_TitleBgActive] = bgLight;
		colors[ImGuiCol_TitleBgCollapsed] = bgDark;

		// 按钮 (霓虹边框感)
		colors[ImGuiCol_Button] = neonPurple * ImVec4(1, 1, 1, 0.2f);
		colors[ImGuiCol_ButtonHovered] = neonPurple * ImVec4(1, 1, 1, 0.6f);
		colors[ImGuiCol_ButtonActive] = neonPurple;

		// 标签页
		colors[ImGuiCol_Tab] = bgDark;
		colors[ImGuiCol_TabHovered] = neonCyan * ImVec4(1, 1, 1, 0.5f);
		colors[ImGuiCol_TabSelected] = neonCyan * ImVec4(1, 1, 1, 0.2f);
		colors[ImGuiCol_TabSelectedOverline] = neonCyan; // 青色顶线

		// 选中高亮
		colors[ImGuiCol_Header] = neonPurple * ImVec4(1, 1, 1, 0.4f);
		colors[ImGuiCol_HeaderHovered] = neonPurple * ImVec4(1, 1, 1, 0.7f);
		colors[ImGuiCol_HeaderActive] = neonPurple;

		colors[ImGuiCol_CheckMark] = neonCyan;
		colors[ImGuiCol_SliderGrab] = neonCyan;
		colors[ImGuiCol_SliderGrabActive] = neonCyan * ImVec4(1.2f, 1.2f, 1.2f, 1.0f);

		colors[ImGuiCol_Separator] = neonPurple * ImVec4(1, 1, 1, 0.5f);
	}

}