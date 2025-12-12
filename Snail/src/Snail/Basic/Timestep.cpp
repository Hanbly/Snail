#include "SNLpch.h"

#include "Snail/Render/Renderer/RendererCommand.h"

#include "Timestep.h"

namespace Snail {

    float Timestep::GetTime()
    {
        switch (RendererCommand::GetWindowAPI()) {
            case RendererCommand::WindowAPI::None:		SNL_CORE_ASSERT(false, "Timestep: 取无效值 None!"); return 0;
            case RendererCommand::WindowAPI::GLFW:		return (float)glfwGetTime();
        }

        SNL_CORE_ASSERT(false, "Timestep: switch无法取值!");
        return 0;
    }

}