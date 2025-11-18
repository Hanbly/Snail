#pragma once

#ifdef SNL_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <functional>

#include <string>
#include <vector>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

#include "Snail/Logger/Log.h"
#include "Snail/Events/Event.h"
