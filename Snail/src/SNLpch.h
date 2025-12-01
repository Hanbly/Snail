#pragma once

#ifdef SNL_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <iostream>
#include <cstdint>
#include <sstream>
#include <memory>
#include <algorithm>
#include <functional>

#include <string>
#include <vector>

#include "Snail/InputCodes.h"
#include "Snail/Timestep.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

#include "Snail/Logger/Log.h"
#include "Snail/Events/Event.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui.h"