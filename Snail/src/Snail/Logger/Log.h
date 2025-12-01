#pragma once

#include "Snail/Core/Core.h"

#include "spdlog/spdlog.h"

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

// ...与__VA_ARGS__ 是一组可变参数的宏
// Snail 端宏
#define SNL_CORE_TRACE(...)		::Snail::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SNL_CORE_INFO(...)		::Snail::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SNL_CORE_WARN(...)		::Snail::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SNL_CORE_ERROR(...)		::Snail::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SNL_CORE_FATAL(...)		::Snail::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// 应用端宏
#define SNL_TRACE(...)			::Snail::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SNL_INFO(...)			::Snail::Log::GetClientLogger()->info(__VA_ARGS__)
#define SNL_WARN(...)			::Snail::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SNL_ERROR(...)			::Snail::Log::GetClientLogger()->error(__VA_ARGS__)
#define SNL_FATAL(...)			::Snail::Log::GetClientLogger()->fatal(__VA_ARGS__)
