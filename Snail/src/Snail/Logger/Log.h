#pragma once

#include "Snail/Basic/Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ranges.h>

namespace Snail {

	class SNAIL_API Log {
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	public:
		static void InitLoggers();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() {
			return s_CoreLogger;
		}
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() {
			return s_ClientLogger;
		}
	};

}
