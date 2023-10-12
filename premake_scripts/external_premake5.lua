-- =============================================================================
-- ROOT_PATH/../external
-- =============================================================================

local WINDOWS = (os.target() == "windows")
local LINUX = (os.target() == "linux")

--------------------------------------------------------------------
-- RANGE-V3
--------------------------------------------------------------------
RANGE_V3_PATH = "range-v3"
RANGE_V3_INC_PATH = "include"
build:DeclareExternalLib("RANGE_V3")

--------------------------------------------------------------------
-- OPENGL
--------------------------------------------------------------------
OPENGL_PATH = "openGL"
OPENGL_INC_PATH = ""
OPENGL_LIB_NAME = "OpenGL32"
build:DeclareExternalLib("OPENGL")

--------------------------------------------------------------------
-- GLM
--------------------------------------------------------------------
GLM_PATH = "glm"
GLM_INC_PATH = ""
build:DeclareExternalLib("GLM")

--------------------------------------------------------------------
-- BOOST
--------------------------------------------------------------------
BOOST_PATH     = "boost_1_83_0"
BOOST_INC_PATH = ""
BOOST_LIB_PATH =  path.join("stage", "lib")
build:DeclareExternalLib("BOOST")

--------------------------------------------------------------------
-- LUA
--------------------------------------------------------------------
LUA_PATH     = "lua-5.4.2"
LUA_INC_PATH = "include"
LUA_LIB_PATH = ""
LUA_LIB_NAME  = "lua54.lib"
build:DeclareExternalLib("LUA")

--------------------------------------------------------------------
-- GLFW
--------------------------------------------------------------------

--[[

GLFW_PATH = "glfw"
GLFW_INC_PATH = "include"
GLFW_LIB_PATH = "lib-vc2022"
GLFW_LIB_NAME = "glfw3dll.lib"
GLFW_TOCOPY  = path.join(GLFW_LIB_PATH, "glfw3.dll")
build:DeclareExternalLib("GLFW")

]]--

GLFW_PATH = "glfw-3.3.8"
GLFW_INC_PATH = "include"
GLFW_LIB_PATH = path.join("src", "Release")
GLFW_LIB_NAME = "glfw3.lib"
GLFW_TOCOPY  = path.join(GLFW_LIB_PATH)
build:DeclareExternalLib("GLFW")

--------------------------------------------------------------------
-- GLSLANG
--------------------------------------------------------------------
GLSLANG_PATH = path.join("glslang", "install")
GLSLANG_INC_PATH = "include"
GLSLANG_LIB_PATH = "lib"

GLSLANG_LIBS = {

	DEBUG = {
		"glslangd.lib",
		"MachineIndependentd.lib",
		"GenericCodeGend.lib",
		"glslang-default-resource-limitsd.lib",
		"HLSLd.lib",
		"OGLCompilerd.lib",
		"OSDependentd.lib",
		"SPIRVd.lib",
		"SPIRV-Toolsd.lib",
		"SPIRV-Tools-diffd.lib",
		"SPIRV-Tools-linkd.lib",
		"SPIRV-Tools-lintd.lib",
		"SPIRV-Tools-optd.lib",
		"SPIRV-Tools-reduced.lib",
		"SPIRV-Tools-sharedd.lib",
		"SPVRemapperd.lib"
	},
	RELEASE = {
		"glslang.lib",
		"MachineIndependent.lib",
		"GenericCodeGen.lib",
		"glslang-default-resource-limits.lib",
		"HLSL.lib",
		"OGLCompiler.lib",
		"OSDependent.lib",
		"SPIRV.lib",
		"SPIRV-Tools.lib",
		"SPIRV-Tools-diff.lib",
		"SPIRV-Tools-link.lib",
		"SPIRV-Tools-lint.lib",
		"SPIRV-Tools-opt.lib",
		"SPIRV-Tools-reduce.lib",
		"SPIRV-Tools-shared.lib",
		"SPVRemapper.lib"
	}
}

build:DeclareExternalLib("GLSLANG")

--------------------------------------------------------------------
-- VISUAL STUDIO REDISTRIBUABLE
--------------------------------------------------------------------
MSVC_PATH = "MSVC_Redist"  
MSVC_BASELIB_PATH = "14.23.27820"
MSVC_LIB_PATH_X64_RELEASE = path.join(MSVC_BASELIB_PATH, "x64", "Microsoft.VC142.CRT") 
MSVC_LIB_PATH_X64_DEBUG = path.join(MSVC_BASELIB_PATH, "debug_nonredist", "x64", "Microsoft.VC142.DebugCRT") 

MSVC_DLL_DEBUG = {
	"concrt140d.dll",
	"msvcp140d.dll",
	"msvcp140_1d.dll",
	"msvcp140_2d.dll",
	"msvcp140d_codecvt_ids.dll",
	"vccorlib140d.dll",
	"vcruntime140d.dll",
	"vcruntime140_1d.dll" -- not for x32, but copy should be ignored
}

MSVC_DLL_RELEASE = {
	"concrt140.dll",
	"msvcp140.dll",
	"msvcp140_1.dll",
	"msvcp140_2.dll",
	"msvcp140_codecvt_ids.dll",
	"vccorlib140.dll",
	"vcruntime140.dll",
	"vcruntime140_1.dll"
}

function GenerateMSVCDLLs(base_path, dlls)

  local result = {}
  for k, v in ipairs(dlls) do
    table.insert(result, path.join(base_path, v))
  end
  return result
end

MSVC_TOCOPY  = {
   DEBUG = GenerateMSVCDLLs(MSVC_LIB_PATH_X64_DEBUG, MSVC_DLL_DEBUG),
   RELEASE = GenerateMSVCDLLs(MSVC_LIB_PATH_X64_RELEASE, MSVC_DLL_RELEASE)
}
build:DeclareExternalLib("MSVC")

--------------------------------------------------------------------
-- GLEW
--------------------------------------------------------------------
GLEW_PATH     = "glew-2.2.0"
GLEW_INC_PATH = "include"
GLEW_LIB_PATH = path.join("lib", RELEASE, x64)
GLEW_LIB_NAME = "glew32.lib"
GLEW_TOCOPY  = path.join("bin", RELEASE, x64, "glew32.dll")  
build:DeclareExternalLib("GLEW")

--------------------------------------------------------------------
-- FREETYPE2
--------------------------------------------------------------------
FREETYPE2_PATH = "freetype2"
FREETYPE2_INC_PATH = "include"
FREETYPE2_LIB_PATH = path.join("objs", x64, "Release Static")
FREETYPE2_LIB_NAME = "freetype.lib"
build:DeclareExternalLib("FREETYPE2")

--------------------------------------------------------------------
-- IRRKLANG
--------------------------------------------------------------------
IRRKLANG_PATH = "irrKlang-64bit-1.6.0"
IRRKLANG_INC_PATH = "include"
IRRKLANG_LIB_PATH = path.join("lib", "Winx64-visualStudio")
IRRKLANG_LIB_NAME = "irrKlang.lib"
IRRKLANG_TOCOPY  = path.join("bin", "winx64-visualStudio" , "irrKlang.dll")  

build:DeclareExternalLib("IRRKLANG")

--------------------------------------------------------------------
-- FBX
--------------------------------------------------------------------
FBX_PATH = path.join("FBX-2020.0.1") 
FBX_INC_PATH = "include"
FBX_LIB_PATH = path.join("lib", "vs2017", "x64", "release")
FBX_LIB_NAME = "libfbxsdk.lib"
FBX_TOCOPY  = path.join(FBX_LIB_PATH, "libfbxsdk.dll")
build:DeclareExternalLib("FBX")

--------------------------------------------------------------------
-- ZLIB
--------------------------------------------------------------------
ZLIB_PATH     = "zlib-1.3"
ZLIB_INC_PATH = ""
ZLIB_LIB_PATH = path.join("contrib",   "vstudio", "vc14", "x64", "ZlibStatRelease")
ZLIB_LIB_NAME = "zlibstat.lib"
build:DeclareExternalLib("ZLIB")

--------------------------------------------------------------------
-- ASSIMP
--------------------------------------------------------------------
ASSIMP_PATH     = "assimp"
ASSIMP_INC_PATH = "include"
ASSIMP_LIB_PATH = path.join("lib", "Release")
ASSIMP_LIB_NAME  = "assimp-vc143-mt.lib"
ASSIMP_TOCOPY   = path.join("bin", "Release", "assimp-vc143-mt.dll")
build:DeclareExternalLib("ASSIMP")

--------------------------------------------------------------------
-- FREEIMAGE
--------------------------------------------------------------------
FREEIMAGE_PATH       = "FreeImage-3.18"
FREEIMAGE_DIST_PATH  = path.join("Dist", "x64")
FREEIMAGE_INC_PATH = FREEIMAGE_DIST_PATH
FREEIMAGE_LIB_PATH = FREEIMAGE_DIST_PATH
FREEIMAGE_LIB_NAME = "FreeImage.lib"
FREEIMAGE_TOCOPY  = path.join(FREEIMAGE_LIB_PATH, "FreeImage.dll")
build:DeclareExternalLib("FREEIMAGE")

--------------------------------------------------------------------
-- LIBXML2
--------------------------------------------------------------------
TINYXML2_PATH = "tinyxml2"
TINYXML2_INC_PATH = ""
TINYXML2_LIB_PATH = {
	DEBUG = DEBUG,
	RELEASE = RELEASE
}
TINYXML2_LIB_NAME = "tinyxml2.lib"
build:DeclareExternalLib("TINYXML2")

--------------------------------------------------------------------
-- JSON
--------------------------------------------------------------------
JSON_PATH     = "json"
JSON_INC_PATH = path.join("single_include", "nlohmann")
build:DeclareExternalLib("JSON")

--------------------------------------------------------------------
-- IMGUI
--------------------------------------------------------------------
IMGUI_PATH     = "imgui"
IMGUI_INC_PATH = "."
IMGUI_LIB_PATH = path.join("bin", RELEASE)
IMGUI_LIB_NAME = "ImGUIOpenGL.lib"

build:DeclareExternalLib("IMGUI")

--------------------------------------------------------------------
-- VULKAN
--------------------------------------------------------------------
VULKAN_PATH = path.join("VulkanSDK", "1.3.261.1")
VULKAN_INC_PATH = "Include"
VULKAN_LIB_PATH = "Lib"
VULKAN_BIN_PATH = "Bin"
VULKAN_LIB_NAME = {"vulkan-1.lib"} --, "VkLayer_utils.lib"}

VULKAN_TOCOPY  = {
	path.join(VULKAN_BIN_PATH, "VkLayer_api_dump.dll"),
	path.join(VULKAN_BIN_PATH, "VkLayer_khronos_validation.dll")
} 

build:DeclareExternalLib("VULKAN")