#pragma once

/**
 * Common "macros"
 */

#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif


template<typename T>
T min(T a, T b)
{
  return (a <= b)? a : b;
}

template<typename T>
T max(T a, T b)
{
  return (a >= b)? a : b;
}

/**
 * This file only deserve to include most external common includes
 */

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4091) // warning C4091: 'typedef ': ignored on left of '' when no variable is declared  (CAUSED by dbghelp.h)
#endif

#define NOMINMAX // remove min(...) and max(...) macro
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
#include <set>
#include <cmath>
#include <cfloat>
#include <io.h>
#include <fcntl.h>
#include <thread>
#include <chrono>
#include <forward_list>
#include <type_traits>

#include <boost/static_assert.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/timer/timer.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/call_traits.hpp>
#include <boost/crc.hpp> 
#include <boost/type_traits.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/intrusive_ptr.hpp>

#include <boost/log/utility/functional/nop.hpp>

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

#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
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
#  include <Shlobj.h>
#  include <Dbghelp.h>
#endif

#include <lua.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H


#include <json.hpp>

#include <tinyxml2.h>

#include <FreeImage.h>

#include <irrKlang.h>

#include <nana/gui.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/msgbox.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/frame.hpp>
#include <nana/gui/wvl.hpp>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>           
#include <assimp/postprocess.h>   

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include <fbxsdk/scene/fbxscene.h>

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

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>

// XXX : GL.h requires windows.h
#include <GL/GL.h>
#include <GL/GLU.h>

// GLFW

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>


#ifdef _MSC_VER
#  pragma warning(pop)
#endif



