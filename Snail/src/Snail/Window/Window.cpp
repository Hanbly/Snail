#include "SNLpch.h"

#include "Window.h"
#include "Platform/glfw/Windows/Window/WindowsWindow.h"

namespace Snail {

    Uniptr<Window> Window::Create(const WindowProps& props)
    {
        // 之前可以switch选择不同的操作系统（平台）调用不同的构造方法
        return std::make_unique<WindowsWindow>(props);
    }

}