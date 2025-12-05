#include "SNLpch.h"

#include "OpenGLRenderContext.h"

namespace Snail {

    bool OpenGLRenderContext::s_GLADInitialized = false;

    OpenGLRenderContext::OpenGLRenderContext(GLFWwindow* window)
    {
        SNL_PROFILE_FUNCTION();


        m_WindowHandle = window;
        OpenGLRenderContext::s_GLADInitialized = false;

        SNL_CORE_ASSERT(m_WindowHandle, "渲染上下文类构造失败, 窗口句柄为空!");
    }

    OpenGLRenderContext::~OpenGLRenderContext()
    {
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

            SNL_CORE_INFO("OpenGL Info: 已创建 OpenGL 渲染器上下文");
            SNL_CORE_INFO("供应商: {0}", (const char*)glGetString(GL_VENDOR));
            SNL_CORE_INFO("渲染设备: {0}", (const char*)glGetString(GL_RENDERER));
            SNL_CORE_INFO("版本号: {0}", (const char*)glGetString(GL_VERSION));
        }
    }

    void OpenGLRenderContext::SwapBuffersImpl()
    {
        // 交换缓冲区
        glfwSwapBuffers(m_WindowHandle);
    }

}
