#include "SNLpch.h"

#include "Snail/Events/ApplicationEvent.h"
#include "Snail/Events/KeyboardEvent.h"
#include "Snail/Events/MouseEvent.h"

#include "WindowsWindow.h"

namespace Snail {

    static bool s_GLFWInitialized = false;
    static bool s_GLADInitialized = false;

    static void GLFWErrorCallback(int errorcode, const char* description) {
        SNL_CORE_ERROR("GLFW ERROR({0}): {1}", errorcode, description);
    }

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

            glfwSetErrorCallback(GLFWErrorCallback);

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

        // 设置GLFW回调方法（由GLFW在事件发生时自动调用的代码块）
        glfwSetKeyCallback(m_Window,
            [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                switch (action) {
                    case GLFW_RELEASE: {
                        KeyReleaseEvent event(key);
                        data.eventCallbackFn(event);
                        break;
                    }
                    case GLFW_PRESS: {
                        KeyPressEvent event(key, 0);
                        data.eventCallbackFn(event);
                        break;
                    }
                    case GLFW_REPEAT: {
                        KeyPressEvent event(key, 1);
                        data.eventCallbackFn(event);
                        break;
                    }
                }
            });
        glfwSetMouseButtonCallback(m_Window,
            [](GLFWwindow* window, int button, int action, int mods) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                switch (action) {
                    case GLFW_RELEASE: {
                        MouseReleaseEvent event(button);
                        data.eventCallbackFn(event);
                        break;
                    }
                    case GLFW_PRESS: {
                        MousePressEvent event(button);
                        data.eventCallbackFn(event);
                        break;
                    }
                }
            });
        glfwSetCursorPosCallback(m_Window,
            [](GLFWwindow* window, double xPos, double yPos) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                MouseMoveEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
                data.eventCallbackFn(event);
            });
        glfwSetScrollCallback(m_Window,
            [](GLFWwindow* window, double xOffset, double yOffset) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                MouseScrollEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
                data.eventCallbackFn(event);
            });
        glfwSetWindowSizeCallback(m_Window,
            [](GLFWwindow* window, int width, int height) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                data.props.width = width;
                data.props.height = height;

                WindowResizeEvent event(width, height);
                data.eventCallbackFn(event);
            });
        glfwSetWindowPosCallback(m_Window,
            [](GLFWwindow* window, int xPos, int yPos) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                WindowMoveEvent event(static_cast<unsigned int>(xPos), static_cast<unsigned int>(yPos));
                data.eventCallbackFn(event);
            });
        glfwSetWindowFocusCallback(m_Window,
            [](GLFWwindow* window, int focused) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                switch (focused) {
                    case GLFW_TRUE: {
                        WindowFocusEvent event;
                        data.eventCallbackFn(event);
                        break;
                    }
                    case GLFW_FALSE: {
                        WindowLostFocusEvent event;
                        data.eventCallbackFn(event);
                        break;
                    }
                }
            });
        glfwSetWindowCloseCallback(m_Window,
            [](GLFWwindow* window) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                WindowCloseEvent event;
                data.eventCallbackFn(event);
            });
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