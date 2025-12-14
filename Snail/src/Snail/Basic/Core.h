#pragma once

#include <string>
#include <memory>

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

namespace Snail {

	template<typename T>
	using Refptr = std::shared_ptr<T>;

	template<typename T>
	using Uniptr = std::unique_ptr<T>;

	struct ProfileResult {
		std::string name;
		long long start, end;
		uint64_t threadID;
	};
}
