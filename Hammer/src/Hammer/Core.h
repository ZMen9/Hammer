#pragma once

#ifdef HM_PLATFORM_WINDOWS
	#ifdef HM_BUILD_DLL
		#define HAMMER_API __declspec(dllexport)
	#else
		#define HAMMER_API __declspec(dllimport)
	#endif
#else
	#error HAMMER ONLY SUPPORTS WINDOWS!
#endif