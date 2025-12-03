#pragma once

#ifdef SNL_PLATFORM_WINDOWS
	#ifdef SNL_BUILD_DLL
		#define SNAIL_API __declspec(dllexport)
	#elif SNL_BUILD_SLL
		#define SNAIL_API
	#else
		#define SNAIL_API
	#endif
#else
	
#endif

#define BIT(x) (1 << x)

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

namespace Snail {

	template<typename T>
	using Refptr = std::shared_ptr<T>;

	template<typename T>
	using Uniptr = std::unique_ptr<T>;

}
