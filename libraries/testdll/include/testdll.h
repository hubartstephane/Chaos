#pragma once

// __declspec(dllexport) seems necessary
// not __declspec(dllimport)

#ifndef TESTDLL_API
	#if defined CHAOS_BUILDING_CHAOS2
		#define TESTDLL_API __declspec(dllexport)
	#else
		#define TESTDLL_API __declspec(dllimport)
	#endif
#endif
