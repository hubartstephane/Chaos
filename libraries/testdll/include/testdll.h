#pragma once


#ifndef TESTDLL_API
#if 1
	#if defined CHAOS_BUILD_DLL
		#define TESTDLL_API __declspec(dllexport)
	#else
		#define TESTDLL_API __declspec(dllimport)
	#endif
#else
	#define TESTDLL_API
#endif
#endif
