-- =============================================================================
-- ROOT_PATH/../external
-- =============================================================================

local WINDOWS = (os.target() == "windows")
local LINUX = (os.target() == "linux")

function MakePathPerConfig(p)

	return {
		DEBUG = path.join("Debug", p),
		RELEASE = path.join("Release", p)
	}

end

--------------------------------------------------------------------
-- GLM
--------------------------------------------------------------------

if WINDOWS then
	GLM_PATH = "glm"
	GLM_INC_PATH = MakePathPerConfig("include")
end
build:DeclareExternalLib("GLM")

--------------------------------------------------------------------
-- TINYXML2
--------------------------------------------------------------------

if WINDOWS then
	TINYXML2_PATH = "tinyxml2"
	TINYXML2_INC_PATH = MakePathPerConfig("include")
	TINYXML2_LIB_PATH = MakePathPerConfig("lib")
	TINYXML2_LIB_NAME = "tinyxml2.lib"
end
if LINUX then
	TINYXML2_LIB_NAME = "tinyxml2"
end
build:DeclareExternalLib("TINYXML2")

--------------------------------------------------------------------
-- JSON
--------------------------------------------------------------------

if WINDOWS then
	JSON_PATH     = "json"
	JSON_INC_PATH = MakePathPerConfig(path.join("include", "nlohmann"))
end
if LINUX then
	JSON_BASE_PATH = path.join("/","usr", "include")
	JSON_INC_PATH = "nlohmann"
end
build:DeclareExternalLib("JSON")

--------------------------------------------------------------------
-- GLFW
--------------------------------------------------------------------

if WINDOWS then
	GLFW_PATH = "glfw"
	GLFW_INC_PATH = MakePathPerConfig("include")
	GLFW_LIB_PATH = MakePathPerConfig("lib")
	GLFW_LIB_NAME = "glfw3.lib"
end
if LINUX then
	GLFW_LIB_NAME = "glfw"
end
build:DeclareExternalLib("GLFW")

--------------------------------------------------------------------
-- OPENGL
--------------------------------------------------------------------

if WINDOWS then
	OPENGL_PATH = "openGL"
	OPENGL_INC_PATH = ""
	OPENGL_LIB_NAME = "OpenGL32"
end
if LINUX then
	OPENGL_LIB_NAME = "GL"
end
build:DeclareExternalLib("OPENGL")

--------------------------------------------------------------------
-- FREEIMAGE
--------------------------------------------------------------------

if WINDOWS then
	FREEIMAGE_PATH       = "FreeImage"
	FREEIMAGE_DIST_PATH  = path.join("Dist", "x64")
	FREEIMAGE_INC_PATH = FREEIMAGE_DIST_PATH
	FREEIMAGE_LIB_PATH = FREEIMAGE_DIST_PATH
	FREEIMAGE_LIB_NAME = "FreeImage.lib"
	FREEIMAGE_TOCOPY  = path.join(FREEIMAGE_LIB_PATH, "FreeImage.dll")
end
if LINUX then
	FREEIMAGE_LIB_NAME = "freeimage"
end
build:DeclareExternalLib("FREEIMAGE")

--------------------------------------------------------------------
-- GLEW
--------------------------------------------------------------------

if WINDOWS then
	GLEW_PATH     = "glew-2.2.0"
	GLEW_INC_PATH = "include"
	GLEW_LIB_PATH = path.join("lib", RELEASE, x64)
	GLEW_LIB_NAME = "glew32.lib"
	GLEW_TOCOPY  = path.join("bin", RELEASE, x64, "glew32.dll")  
end
if LINUX then
	GLEW_LIB_NAME = "GLEW"
end
build:DeclareExternalLib("GLEW")

--------------------------------------------------------------------
-- LUA
--------------------------------------------------------------------

if WINDOWS then
	LUA_PATH     = "lua-5.4.2"
	LUA_INC_PATH = "include"
	LUA_LIB_PATH = ""
	LUA_LIB_NAME  = "lua54.lib"
end
if LINUX then
	LUA_BASE_PATH = path.join("/","usr", "include")
	LUA_INC_PATH = "lua5.4"
	LUA_LIB_NAME  = "lua5.4"
end
build:DeclareExternalLib("LUA")

--------------------------------------------------------------------
-- FREETYPE2
--------------------------------------------------------------------

if WINDOWS then
	FREETYPE2_PATH = "freetype2"
	FREETYPE2_INC_PATH = MakePathPerConfig(path.join("include", "freetype2"))
	FREETYPE2_LIB_PATH = MakePathPerConfig("lib")
	FREETYPE2_LIB_NAME =
	{
		DEBUG = "freetyped.lib",
		RELEASE = "freetype.lib",
	}
end
if LINUX then
	FREETYPE2_BASE_PATH = path.join("/","usr", "include")
	FREETYPE2_INC_PATH = "freetype2"
	FREETYPE2_LIB_NAME = "freetype"
end
local freetype2 = build:DeclareExternalLib("FREETYPE2")
freetype2:DependOnLib("ZLIB")

--------------------------------------------------------------------
-- ASSIMP
--------------------------------------------------------------------

if WINDOWS then
	ASSIMP_PATH     = "assimp"
	ASSIMP_INC_PATH = MakePathPerConfig("include")
	ASSIMP_LIB_PATH = MakePathPerConfig("lib")
	ASSIMP_LIB_NAME = {
		DEBUG = "assimp-vc143-mtd.lib",
		RELEASE = "assimp-vc143-mt.lib"
	}
	ASSIMP_TOCOPY   = {
		DEBUG = path.join("Debug", "bin", "assimp-vc143-mtd.dll"),
		RELEASE = path.join("Release", "bin", "assimp-vc143-mt.dll")
	}
end
if LINUX then
	ASSIMP_BASE_PATH = path.join("/","usr", "include")
	ASSIMP_INC_PATH = "assimp"
	ASSIMP_LIB_NAME = "assimp"
end
build:DeclareExternalLib("ASSIMP")

--------------------------------------------------------------------
-- GLSLANG
--------------------------------------------------------------------

if WINDOWS then

	VULKAN_PATH = os.getenv("VULKAN_SDK")

	GLSLANG_PATH = VULKAN_PATH
	GLSLANG_INC_PATH = "include"
	GLSLANG_LIB_PATH = "lib"
	GLSLANG_LIB_NAME = {
		DEBUG = {
			"glslangd.lib",
			"MachineIndependentd.lib",
			"GenericCodeGend.lib",
			"glslang-default-resource-limitsd.lib",
			--"HLSLd.lib",
			--"OGLCompilerd.lib",
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
		--	"HLSL.lib",
		--	"OGLCompiler.lib",
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
end
if LINUX then
	GLSLANG_BASE_PATH = path.join("/","usr", "include")
	GLSLANG_INC_PATH = "glslang"
	GLSLANG_LIB_NAME = {"glslang", "MachineIndependent", "OSDependent", "HLSL", "OGLCompiler", "GenericCodeGen", "SPVRemapper", "pthread"}
end
build:DeclareExternalLib("GLSLANG")

--------------------------------------------------------------------
-- VULKAN
--------------------------------------------------------------------

if WINDOWS then
	VULKAN_PATH = os.getenv("VULKAN_SDK")

	VULKAN_INC_PATH = "Include"
	VULKAN_LIB_PATH = "Lib"
	VULKAN_BIN_PATH = "Bin"
	VULKAN_LIB_NAME = {"vulkan-1.lib"} --, "VkLayer_utils.lib"}
	VULKAN_TOCOPY  = {
		path.join(VULKAN_BIN_PATH, "VkLayer_api_dump.dll"),
		path.join(VULKAN_BIN_PATH, "VkLayer_khronos_validation.dll")
	} 
end
if LINUX then
	VULKAN_BASE_PATH = path.join("/","usr", "include")
	VULKAN_INC_PATH = "vulkan"
	VULKAN_LIB_NAME = "vulkan"
end
build:DeclareExternalLib("VULKAN")

--------------------------------------------------------------------
-- ZLIB
--------------------------------------------------------------------

if WINDOWS then
	ZLIB_PATH     = "zlib-1.3"
	ZLIB_INC_PATH = MakePathPerConfig("include")
	ZLIB_LIB_PATH = MakePathPerConfig("lib")
	ZLIB_LIB_NAME = {
		DEBUG = "zlibstaticd.lib",
		RELEASE = "zlibstatic.lib"
	}
end
if LINUX then
	ZLIB_BASE_PATH = path.join("/","usr", "include")
	ZLIB_INC_PATH = "zlib"
	ZLIB_LIB_NAME = "z"
end
build:DeclareExternalLib("ZLIB")

--------------------------------------------------------------------
-- BOOST
--------------------------------------------------------------------

if WINDOWS then
	BOOST_PATH     = "boost_1_83_0"
	BOOST_INC_PATH = path.join("include", "boost-1_83")
	BOOST_LIB_PATH = "lib"
end
if LINUX then
	BOOST_LIB_NAME = {"boost_system", "boost_thread", "boost_chrono", "boost_atomic", "boost_filesystem", "boost_timer", "boost_program_options", "boost_log"}
end

build:DeclareExternalLib("BOOST")

--------------------------------------------------------------------
-- VISUAL STUDIO REDISTRIBUABLE
--------------------------------------------------------------------

if WINDOWS then
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
end

--------------------------------------------------------------------
-- IRRKLANG
--------------------------------------------------------------------

if WINDOWS then
	IRRKLANG_PATH = "irrKlang-64bit-1.6.0"
	IRRKLANG_INC_PATH = "include"
	IRRKLANG_LIB_PATH = path.join("lib", "Winx64-visualStudio")
	IRRKLANG_LIB_NAME = "irrKlang.lib"
	IRRKLANG_TOCOPY  = path.join("bin", "winx64-visualStudio" , "irrKlang.dll")  
end
if LINUX then
	IRRKLANG_PATH = "irrKlang-64bit-1.6.0"
	IRRKLANG_INC_PATH = "include"
	IRRKLANG_LINKOPTIONS = path.join(EXTERNAL_PATH, "irrKlang-64bit-1.6.0", "bin", "linux-gcc-64", "libIrrKlang.so")
end

build:DeclareExternalLib("IRRKLANG")

--------------------------------------------------------------------
-- IMGUI
--------------------------------------------------------------------

if WINDOWS then
	IMGUI_PATH     = "ImGui"
	IMGUI_INC_PATH = "."
	IMGUI_LIB_PATH = path.join("bin", "windows", "Release")
	IMGUI_LIB_NAME = "ImGUIOpenGL.lib"
end

if LINUX then
	IMGUI_PATH     = "ImGui"
	IMGUI_INC_PATH = "."
	IMGUI_LIB_PATH = path.join("bin", "linux", "Release")
	IMGUI_LIB_NAME = "ImGUIOpenGL"
end

build:DeclareExternalLib("IMGUI")



--[[

--------------------------------------------------------------------
-- GLM
--------------------------------------------------------------------
GLM_PATH = "glm"
GLM_INC_PATH = MakePathPerConfig("include")
build:DeclareExternalLib("GLM")


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
-- BOOST
--------------------------------------------------------------------
BOOST_PATH     = "boost_1_83_0"
BOOST_INC_PATH = path.join("include", "boost-1_83")
BOOST_LIB_PATH = "lib"
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

GLFW_PATH = "glfw"
GLFW_INC_PATH = MakePathPerConfig("include")
GLFW_LIB_PATH = MakePathPerConfig("lib")
GLFW_LIB_NAME = "glfw3.lib"
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
FREETYPE2_INC_PATH = MakePathPerConfig(path.join("include", "freetype2"))
FREETYPE2_LIB_PATH = MakePathPerConfig("lib")
FREETYPE2_LIB_NAME = 
{
	DEBUG = "freetyped.lib",
	RELEASE = "freetype.lib",
}
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
FBX_PATH = path.join("FBX SDK", "2020.0.1") 
FBX_INC_PATH = "include"
FBX_LIB_PATH = path.join("lib", "vs2017", "x64", "release")
FBX_LIB_NAME = "libfbxsdk.lib"
FBX_TOCOPY  = path.join(FBX_LIB_PATH, "libfbxsdk.dll")
build:DeclareExternalLib("FBX")

--------------------------------------------------------------------
-- ZLIB
--------------------------------------------------------------------
ZLIB_PATH     = "zlib-1.3"
ZLIB_INC_PATH = MakePathPerConfig("include")
ZLIB_LIB_PATH = MakePathPerConfig("lib")
ZLIB_LIB_NAME = {
	DEBUG = "zlibstaticd.lib",
	RELEASE = "zlibstatic.lib"
}
build:DeclareExternalLib("ZLIB")

--------------------------------------------------------------------
-- ASSIMP
--------------------------------------------------------------------
ASSIMP_PATH     = "assimp"
ASSIMP_INC_PATH = MakePathPerConfig("include")
ASSIMP_LIB_PATH = MakePathPerConfig("lib")
ASSIMP_LIB_NAME = {
	DEBUG = "assimp-vc143-mtd.lib",
	RELEASE = "assimp-vc143-mt.lib"
}
ASSIMP_TOCOPY   = {
	DEBUG = path.join("Debug", "bin", "assimp-vc143-mtd.dll"),
	RELEASE = path.join("Release", "bin", "assimp-vc143-mt.dll")
}
build:DeclareExternalLib("ASSIMP")

--------------------------------------------------------------------
-- FREEIMAGE
--------------------------------------------------------------------
FREEIMAGE_PATH       = "FreeImage"
FREEIMAGE_DIST_PATH  = path.join("Dist", "x64")
FREEIMAGE_INC_PATH = FREEIMAGE_DIST_PATH
FREEIMAGE_LIB_PATH = FREEIMAGE_DIST_PATH
FREEIMAGE_LIB_NAME = "FreeImage.lib"
FREEIMAGE_TOCOPY  = path.join(FREEIMAGE_LIB_PATH, "FreeImage.dll")
build:DeclareExternalLib("FREEIMAGE")

--------------------------------------------------------------------
-- TINYXML2
--------------------------------------------------------------------
TINYXML2_PATH = "tinyxml2"
TINYXML2_INC_PATH = MakePathPerConfig("include")
TINYXML2_LIB_PATH = MakePathPerConfig("lib")
TINYXML2_LIB_NAME = "tinyxml2.lib"
build:DeclareExternalLib("TINYXML2")

--------------------------------------------------------------------
-- JSON
--------------------------------------------------------------------
JSON_PATH     = "json"
JSON_INC_PATH = MakePathPerConfig(path.join("include", "nlohmann"))
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

VULKAN_PATH = os.getenv("VULKAN_SDK")
if (not VULKAN_PATH) then
	VULKAN_PATH = path.join("VulkanSDK", "1.3.261.1")
end
VULKAN_INC_PATH = "Include"
VULKAN_LIB_PATH = "Lib"
VULKAN_BIN_PATH = "Bin"
VULKAN_LIB_NAME = {"vulkan-1.lib"} --, "VkLayer_utils.lib"}

VULKAN_TOCOPY  = {
	path.join(VULKAN_BIN_PATH, "VkLayer_api_dump.dll"),
	path.join(VULKAN_BIN_PATH, "VkLayer_khronos_validation.dll")
} 

build:DeclareExternalLib("VULKAN")

]]--
