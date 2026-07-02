#pragma once

#ifdef _WIN32
	#ifdef ENGINE_CORE_EXPORTS
		#define ENGINE_CORE_API __declspec(dllexport)
	#else
		#define ENGINE_CORE_API __declspec(dllimport)
	#endif
#else
	#error "Unsupported platform"
#endif

