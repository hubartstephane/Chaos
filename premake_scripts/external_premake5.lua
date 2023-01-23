-- =============================================================================
-- ROOT_PATH/../external
-- =============================================================================

local WINDOWS = (os.target() == "windows")
local LINUX = (os.target() == "linux")

--------------------------------------------------------------------
-- RANGE-V3
--------------------------------------------------------------------
local RANGE_V3_PATH = "range-v3"
local RANGE_V3_INC_PATH = "include"
build:DeclareExternalLib("range-v3", RANGE_V3_PATH, RANGE_V3_INC_PATH, nil, nil)

--------------------------------------------------------------------
-- OPENGL
--------------------------------------------------------------------
local GL_PATH = "openGL"
local GL_INC_PATH = ""
build:DeclareExternalLib("OPENGL", GL_PATH, GL_INC_PATH, nil, "OpenGL32")

--------------------------------------------------------------------
-- GLM
--------------------------------------------------------------------
local GLM_PATH = "glm"
local GLM_INC_PATH = ""
build:DeclareExternalLib("GLM", GLM_PATH, GLM_INC_PATH, nil, nil)

--------------------------------------------------------------------
-- BOOST
--------------------------------------------------------------------
local BOOST_PATH     = "boost_1_80_0"
local BOOST_INC_PATH = ""
local BOOST_LIB_PATH =  path.join("stage", "lib")
build:DeclareExternalLib("BOOST", BOOST_PATH, BOOST_INC_PATH, BOOST_LIB_PATH, nil)

--------------------------------------------------------------------
-- LUA
--------------------------------------------------------------------
local LUA_PATH     = "lua-5.4.2"
local LUA_INC_PATH = "include"
local LUA_LIB_PATH = ""
build:DeclareExternalLib("LUA", LUA_PATH, LUA_INC_PATH, LUA_LIB_PATH, "lua54.lib")

--------------------------------------------------------------------
-- GLFW
--------------------------------------------------------------------
local GLFW_PATH = "glfw"
local GLFW_INC_PATH = "include"
local GLFW_LIB_PATH = path.join("src", RELEASE)  
build:DeclareExternalLib("GLFW", GLFW_PATH, GLFW_INC_PATH, GLFW_LIB_PATH, "glfw3.lib")

--------------------------------------------------------------------
-- GLSLANG
--------------------------------------------------------------------
local GLSLANG_PATH = path.join("glslang", "install")
local GLSLANG_INC_PATH = "include"
local GLSLANG_LIB_PATH = "lib"

local GLSLANG_LIBS = {

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

build:DeclareExternalLib("GLSLANG", GLSLANG_PATH, GLSLANG_INC_PATH, GLSLANG_LIB_PATH, GLSLANG_LIBS)

--------------------------------------------------------------------
-- VISUAL STUDIO REDISTRIBUABLE
--------------------------------------------------------------------
local MSVC_PATH = "MSVC_Redist"  
local MSVC_BASELIB_PATH = "14.23.27820"
local MSVC_LIB_PATH_X64_RELEASE = path.join(MSVC_BASELIB_PATH, "x64", "Microsoft.VC142.CRT") 
local MSVC_LIB_PATH_X64_DEBUG = path.join(MSVC_BASELIB_PATH, "debug_nonredist", "x64", "Microsoft.VC142.DebugCRT") 

local MSVC_DLL_DEBUG = {
	"concrt140d.dll",
	"msvcp140d.dll",
	"msvcp140_1d.dll",
	"msvcp140_2d.dll",
	"msvcp140d_codecvt_ids.dll",
	"vccorlib140d.dll",
	"vcruntime140d.dll",
	"vcruntime140_1d.dll" -- not for x32, but copy should be ignored
}

local MSVC_DLL_RELEASE = {
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

local MSVC_TOCOPY  = {
   DEBUG = GenerateMSVCDLLs(MSVC_LIB_PATH_X64_DEBUG, MSVC_DLL_DEBUG),
   RELEASE = GenerateMSVCDLLs(MSVC_LIB_PATH_X64_RELEASE, MSVC_DLL_RELEASE)
}
build:DeclareExternalLib("MSVC", MSVC_PATH, nil, nil, nil, MSVC_TOCOPY)

--------------------------------------------------------------------
-- GLEW
--------------------------------------------------------------------
local GLEW_PATH     = "glew-2.2.0"
local GLEW_INC_PATH = "include"
local GLEW_LIB_PATH = path.join("lib", RELEASE, x64)
local GLEW_LIBNAME = "glew32.lib"
local GLEW_TOCOPY  = path.join("bin", RELEASE, x64, "glew32.dll")  
build:DeclareExternalLib("GLEW", GLEW_PATH, GLEW_INC_PATH, GLEW_LIB_PATH, GLEW_LIBNAME, GLEW_TOCOPY)

--------------------------------------------------------------------
-- FREETYPE2
--------------------------------------------------------------------
local FREETYPE2_PATH = "freetype2"
local FREETYPE2_INC_PATH = "include"
local FREETYPE2_LIB_PATH = path.join("objs", x64, "Release Static")
build:DeclareExternalLib("FREETYPE2", FREETYPE2_PATH, FREETYPE2_INC_PATH, FREETYPE2_LIB_PATH, "freetype.lib")

--------------------------------------------------------------------
-- IRRKLANG
--------------------------------------------------------------------
local IRRKLANG_PATH = "irrKlang-64bit-1.6.0"
local IRRKLANG_INC_PATH = "include"
local IRRKLANG_LIB_PATH = path.join("lib", "Winx64-visualStudio")
local IRRKLANG_LIBNAME = "irrKlang.lib"
local IRRKLANG_TOCOPY  = path.join("bin", "winx64-visualStudio" , "irrKlang.dll")  

build:DeclareExternalLib("IRRKLANG", IRRKLANG_PATH, IRRKLANG_INC_PATH, IRRKLANG_LIB_PATH, IRRKLANG_LIBNAME, IRRKLANG_TOCOPY)

--------------------------------------------------------------------
-- FBX
--------------------------------------------------------------------
local FBX_PATH = path.join("FBX-2020.0.1") 
local FBX_INC_PATH = "include"
local FBX_LIB_PATH = path.join("lib", "vs2017", "x64", "release")
local FBX_LIBNAME = "libfbxsdk.lib"
local FBX_TOCOPY  = path.join(FBX_LIB_PATH, "libfbxsdk.dll")
build:DeclareExternalLib("FBX", FBX_PATH, FBX_INC_PATH, FBX_LIB_PATH, FBX_LIBNAME, FBX_TOCOPY)

--------------------------------------------------------------------
-- ZLIB
--------------------------------------------------------------------
local ZLIB_PATH     = "zlib-1.2.11"
local ZLIB_INC_PATH = ""
local ZLIB_LIB_PATH = path.join("contrib",   "vstudio", "vc14", "x64", "ZlibStatRelease")
build:DeclareExternalLib("ZLIB", ZLIB_PATH, ZLIB_INC_PATH, ZLIB_LIB_PATH, "zlibstat.lib", nil)

--------------------------------------------------------------------
-- ASSIMP
--------------------------------------------------------------------
local ASSIMP_PATH     = "assimp"
local ASSIMP_INC_PATH = "include"
local ASSIMP_LIB_PATH = path.join("lib", "Release")
local ASSIMP_LIBNAME  = "assimp-vc143-mt.lib"
local ASSIMP_TOCOPY   = path.join("bin", "Release", "assimp-vc143-mt.dll")
build:DeclareExternalLib("ASSIMP", ASSIMP_PATH, ASSIMP_INC_PATH, ASSIMP_LIB_PATH, ASSIMP_LIBNAME, ASSIMP_TOCOPY)

--------------------------------------------------------------------
-- FREEIMAGE
--------------------------------------------------------------------
local FREEIMAGE_PATH       = "FreeImage-3.18"
local FREEIMAGE_DIST_PATH  = path.join("Dist", "x64")
local FREEIMAGE_INC_PATH = FREEIMAGE_DIST_PATH
local FREEIMAGE_LIB_PATH = FREEIMAGE_DIST_PATH
local FREEIMAGE_LIBNAME = "FreeImage.lib"
local FREEIMAGE_TOCOPY  = path.join(FREEIMAGE_LIB_PATH, "FreeImage.dll")
build:DeclareExternalLib("FREEIMAGE", FREEIMAGE_PATH, FREEIMAGE_INC_PATH, FREEIMAGE_LIB_PATH, FREEIMAGE_LIBNAME, FREEIMAGE_TOCOPY)

--------------------------------------------------------------------
-- LIBXML2
--------------------------------------------------------------------
local LIBTINYXML2_PATH = "tinyxml2"
local LIBTINYXML2_INC_PATH = ""
local LIBTINYXML2_LIB_PATH = {
	DEBUG = path.join("projects", DEBUG),
	RELEASE = path.join("projects", RELEASE)
}
build:DeclareExternalLib("TINYXML2", LIBTINYXML2_PATH, LIBTINYXML2_INC_PATH, LIBTINYXML2_LIB_PATH, "tinyxml2.lib", nil)

--------------------------------------------------------------------
-- NANA
--------------------------------------------------------------------
local NANA_PATH     = "nana-1.7.4"
local NANA_INC_PATH = "include"
local NANA_LIB_PATH = path.join("build", "bin")
local NANA_LIBNAME = {
	DEBUG = "nana_v143_Debug_x64.lib",
	RELEASE = "nana_v143_Release_x64.lib"
}
build:DeclareExternalLib("NANA", NANA_PATH, NANA_INC_PATH, NANA_LIB_PATH, NANA_LIBNAME)

--------------------------------------------------------------------
-- JSON
--------------------------------------------------------------------
local JSON_PATH     = "json"
local JSON_INC_PATH = path.join("single_include", "nlohmann")
build:DeclareExternalLib("JSON", JSON_PATH, JSON_INC_PATH, nil, nil)



	
	--[[
	
  -- VULKAN   
	
local VULKAN_PATH = path.join("VulkanSDK", "1.2.162.0")
local VULKAN_INC_PATH = path.join(VULKAN_PATH, "Include")
local VULKAN_LIB_PATH = path.join(VULKAN_PATH, "Lib")
local VULKAN_BIN_PATH = path.join(VULKAN_PATH, "BIN")
local VULKAN_LIBNAME = {"vulkan-1.lib", "VkLayer_utils.lib"}


local VULKAN_TOCOPY  = { -- @ because this copies the file directly in
    "@" .. path.join(VULKAN_BIN_PATH, "VkLayer_api_dump.*"),
    "@" .. path.join(VULKAN_BIN_PATH, "VkLayer_khronos_validation.*")
} 

DeclareExternalLib("VULKAN", VULKAN_INC_PATH, VULKAN_LIB_PATH, VULKAN_LIBNAME, VULKAN_TOCOPY)

]]--



