#include "SNLpch.h"

#include "Snail/Render/Renderer/RendererCommand.h"

#include "Window.h"
#include "Platform/glfw/Windows/Window/WindowsWindow.h"

namespace Snail {

    Uniptr<Window> Window::Create(const WindowProps& props)
    {
        SNL_PROFILE_FUNCTION();


        switch (RendererCommand::GetWindowAPI()) {
            case RendererCommand::WindowAPI::None:		SNL_CORE_ASSERT(false, "Window: 取无效值 None!"); return nullptr;
            case RendererCommand::WindowAPI::GLFW:		return std::make_unique<WindowsWindow>(props);
        }

        SNL_CORE_ASSERT(false, "Window: switch无法取值!");
        return nullptr;
    }

}