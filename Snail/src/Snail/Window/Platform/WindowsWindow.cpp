#include "SNLpch.h"

#include "WindowsWindow.h"

namespace Snail {

    static bool s_GLFWInitialized = false;
    static bool s_GLADInitialized = false;

    WindowsWindow::WindowsWindow(const WindowProps& props)
    {
        InitWindow(props);
    }

    WindowsWindow::~WindowsWindow()
    {
        ShutdownWindow();
    }

    void WindowsWindow::InitWindow(const WindowProps& props)
    {
        m_Data.props = props;

        if (!s_GLFWInitialized) {
            int status = glfwInit();
            SNL_CORE_ASSERT(status, "GLFW初始化失败");

            s_GLFWInitialized = true;
        }

        const std::string& iTitle = m_Data.props.title;
        const int& iWidth = static_cast<int>(m_Data.props.width);
        const int& iHeight = static_cast<int>(m_Data.props.height);

        m_Window = glfwCreateWindow(iWidth, iHeight, iTitle.c_str(), nullptr, nullptr);
        /* Make the window's context current */
        glfwMakeContextCurrent(m_Window);

        // 初始化glad
        if (!s_GLADInitialized) {
            int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
            SNL_CORE_ASSERT(status, "GLAD初始化失败!");

            s_GLADInitialized = true;
        }

        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true);

        SNL_CORE_INFO("已创建窗口: {0}, 宽度{1}, 高度{2}", iTitle, iWidth, iHeight);
    }

    void WindowsWindow::ShutdownWindow()
    {
        glfwDestroyWindow(m_Window);
    }

    // 轮询事件 & 交换缓冲区
    void WindowsWindow::OnUpdate()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    // cccccccccccccccccccccccc
    void WindowsWindow::SetVSync(bool enable)
    {
        if (enable) {
            glfwSwapInterval(1);
        }
        else {
            glfwSwapInterval(0);
        }

        m_Data.VSync = enable;
    }

    bool WindowsWindow::IsVSync() const
    {
        return m_Data.VSync;
    }

    Window* Window::SNLCreateWindow(const WindowProps& props)
    {
        return new WindowsWindow(props);
    }

}