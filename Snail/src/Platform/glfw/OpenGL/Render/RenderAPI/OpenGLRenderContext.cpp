#include "SNLpch.h"

#include "OpenGLRenderContext.h"

namespace Snail {

    bool OpenGLRenderContext::s_GLADInitialized = false;

    OpenGLRenderContext::OpenGLRenderContext(GLFWwindow* window)
    {
        m_WindowHandle = window;
        OpenGLRenderContext::s_GLADInitialized = false;

        SNL_CORE_ASSERT(m_WindowHandle, "渲染上下文类构造失败, 窗口句柄为空!");
    }

    OpenGLRenderContext::~OpenGLRenderContext()
    {
        glfwDestroyWindow(m_WindowHandle);
    }

    void OpenGLRenderContext::InitContextImpl()
    {
        // 创建窗口的OpenGL上下文
        glfwMakeContextCurrent(m_WindowHandle);
        // 初始化glad
        if (!OpenGLRenderContext::s_GLADInitialized) {
            int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
            SNL_CORE_ASSERT(status, "GLAD初始化失败!");

            OpenGLRenderContext::s_GLADInitialized = true;
        }
    }

    void OpenGLRenderContext::SwapBuffersImpl()
    {
        // 交换缓冲区
        glfwSwapBuffers(m_WindowHandle);
    }

}
