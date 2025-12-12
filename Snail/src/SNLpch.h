#pragma once

#ifdef SNL_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <iostream>
#include <cstdint>
#include <sstream>
#include <filesystem>
#include <memory>
#include <algorithm>
#include <functional>

#include <string>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

#include "Snail/Basic/InputCodes.h"
#include "Snail/Basic/Timestep.h"
#include "Snail/Basic/Timer.h"

#include "Snail/Logger/Log.h"
#include "Snail/Events/Event.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui.h"
