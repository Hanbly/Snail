#pragma once

#ifdef SNL_PLATFORM_WINDOWS
	#ifdef SNL_BUILD_DLL
		#define SNAIL_API __declspec(dllexport)
	#else
		#define SNAIL_API __declspec(dllimport)
	#endif
#else
	
#endif

#define BIT(x) (1 << x)
