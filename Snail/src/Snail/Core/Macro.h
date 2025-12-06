#pragma once

// 注意：不要在以下.h引入 Macro.h，只能引入 Core.h
#include "Snail/Logger/Log.h"
#include "Timer.h"

namespace Snail {

	#define BIT(x) (1 << x)

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

	#ifdef SNL_ENABLED_ASSERTS
	#define SNL_ASSERT(x, ...); { \
										if (!(x)) {\
											SNL_CORE_ERROR("SNL客户端错误断言: {0}", __VA_ARGS__);\
											__debugbreak();\
										}\
									}
	#define SNL_CORE_ASSERT(x, ...); { \
											if(!(x)) {\
												SNL_CORE_ERROR("SNL系统错误断言: {0}", __VA_ARGS__);\
												__debugbreak();\
											}\
										}
	#else
	#define SNL_ASSERT(x, ...);
	#define SNL_CORE_ASSERT(x, ...);
	#endif

	#define BIND_NSTATIC_MEMBER_Fn(x) std::bind(&x, this, std::placeholders::_1)

	#if SNL_PROFILING && 0
	#define SNL_PROFILE_BEGIN_SESSION(name, path) Snail::Instrumentor::BeginSession(name, path)
	#define SNL_PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](Snail::ProfileResult pr){ Snail::s_ProfilingResults.push_back(pr); })
	#define SNL_PROFILE_FUNCTION() Snail::SNL_PROFILE_SCOPE(__FUNCSIG__)
	#define SNL_PROFILE_END_SESSION() Snail::Instrumentor::EndSession()
	#else
	#define SNL_PROFILE_BEGIN_SESSION(name, path);
	#define SNL_PROFILE_SCOPE(name, nullptr);
	#define SNL_PROFILE_FUNCTION();
	#define SNL_PROFILE_END_SESSION();
	#endif

}
