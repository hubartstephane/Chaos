#pragma once

#include "chaos/Config.h"

/**
* Some Initial checks
*/

// must use preprecessor settings:    /Zc:preprocessor
#if !defined __VA_OPT__
#	error "__VA_OPT__ is not enabled: set /Zc:preprocessor flag in Visual Studio project settings"
#endif

/**
* Common "macros"
*/

#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif

#if _WIN32
#	if !_WIN64
#		error "cannot compile in x32. Compile in x64 instead"
#	endif
#endif

/**
* This file only deserve to include most external common includes
*/

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4091) // warning C4091: 'typedef ': ignored on left of '' when no variable is declared  (CAUSED by dbghelp.h)
#endif

#define NOMINMAX // remove min(...) and max(...) macro
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS // due to ostrstream
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <time.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <tuple>
#include <array>
#include <cstdlib>
#include <functional>
#include <algorithm>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <utility>
#include <fstream>
#include <sstream> // for ostringstream
#include <strstream> // for ostrstream (deprecated, will be replaced by spanstream in C++23)
#include <set>
#include <cmath>
#include <cfloat>
#include <random>
#include <concepts>
#include <source_location>

#if _WIN32
#include <io.h>
#endif

#include <fcntl.h>
#include <thread>
#include <future>
#include <chrono>
#include <compare>
#include <forward_list>
#include <type_traits>
#include <atomic>
#include <nmmintrin.h>

// boost is full of #pragma comment(lib, ...)
// ignore theses link directive for STATIC_LIBRARIES that would use this header
#if !defined DEATH_BUILDING_SHARED_LIBRARY && !defined DEATH_BUILDING_EXECUTABLE
#define BOOST_ALL_NO_LIB
#endif
#define BOOST_LIB_DIAGNOSTIC // display all #pragma comment(lib, ...) directives
#include <boost/static_assert.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/timer/timer.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/call_traits.hpp>
#include <boost/type_traits.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range/irange.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/log/utility/functional/nop.hpp>
#include <boost/tti/has_member_data.hpp>
#include <boost/tti/has_member_function.hpp>
#include <boost/convert/detail/has_member.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/circular_buffer.hpp>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/end.hpp>
#include <boost/mpl/not_equal_to.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/inherit_linearly.hpp>
#include <boost/mpl/inherit.hpp>
#include <boost/mpl/empty_base.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/apply_wrap.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/mpl/aux_/has_type.hpp>
#include <boost/mpl/quote.hpp>

#include <boost/preprocessor/facilities/is_empty_variadic.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/logical/or.hpp>
#include <boost/preprocessor/logical/and.hpp>
#include <boost/preprocessor/variadic/size.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/comparison/greater_equal.hpp>
#include <boost/preprocessor/comparison/greater.hpp>
#include <boost/preprocessor/comparison/less_equal.hpp>
#include <boost/preprocessor/comparison/less.hpp>

static_assert(BOOST_PP_IS_EMPTY(), "/Zc:preprocessor flag is required for windows");

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SIZE_T_LENGTH
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/detail/type_vec1.hpp>
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec4.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

#if _WIN32
#  include <intrin.h>
#  include <tchar.h>
#  include <windows.h>
#  include <Windowsx.h>
#  include <Mmsystem.h>
#  include <Shlobj.h>
#  include <Dbghelp.h>
#  include <WinUser.h>

#undef DELETE // thanks to winnt.h we have this define !

#endif // #if _WIN32

#include <lua.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H

// implicit conversion
// type variable = myjson;
//#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include <zlib.h>

#include <tinyxml2.h>

#include <FreeImage.h>

#include <irrKlang.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//#include <fbxsdk.h>
//#include <fbxsdk/fileio/fbxiosettings.h>
//#include <fbxsdk/scene/fbxscene.h>

#include <ranges>

// ==========================================================================================
// The usage of openGL and its libraries can be tedious (mainly for the different inclusions)
// The files deserve to simplify the process into a single inclusion of this file
// ==========================================================================================

// XXX : glew.h MUST be included before GL.h
//       There are 2 interesting macro:
//         - GLEW_MX : replace some functions by macro
//         - GLEW_STATIC : indicates whether the library is statically linked or not
//           => this toggle key words for inclusions as
//              - extern
//              - extern __declspec(dllimport)
//              - extern __declspec(dllexport)   => at library compilation
//
// If chaos is compiled as a DLL, glew cannot be used as a STATIC LIBRARY
// This is because, glew defines somes global variables as pointers on function
// Theses variables are initialized with glewInit(...) function
//
// If chaos is a DLL and glew is STATIC, both executable and chaos will linked to glew
// there will be 2 sets of thoses global variables (duplication), and only the set used in chaos with DLL will be initialized
// If you call in your executable some OpenGL function this would crash

//#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>

// XXX : GL.h requires windows.h
#include <GL/gl.h>
#include <GL/glu.h>

// GLFW
#if WITH_VULKAN
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#else // WITH_VULKAN

//#define GLFW_DLL
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

// IMGUI
#define IMGUI_USE_WCHAR32 // important: imgui should have been compiled with this flag
#include "imgui.h"
#include "imgui_internal.h" // not very nice, but i'd like to have access to ImGui::GetHoveredID()
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

// for forcefeedback
#include "xinput.h"
#pragma comment(lib, "xinput.lib")

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

// IMPLOT
#include "implot.h"