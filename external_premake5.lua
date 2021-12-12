-- =============================================================================
-- ROOT_PATH/../external
-- =============================================================================


NEW_EXTERNAL_PATH = path.join(ROOT_PATH, "external")



local WINDOWS = (os.target() == "windows")
local LINUX = (os.target() == "linux")

  -- RANGE-V3
local RANGE_V3_INC_PATH =  path.join("range-v3", "include")

local tmp = EXTERNAL_PATH
EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("range-v3", RANGE_V3_INC_PATH, nil, nil)  
EXTERNAL_PATH = tmp





  -- OPENGL
local GL_INC_PATH = "openGL"
local tmp = EXTERNAL_PATH
EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("OPENGL", GL_INC_PATH, nil, "OpenGL32")  
EXTERNAL_PATH = tmp
                                             
  -- GLM
local GLM_INC_PATH = "glm"

local tmp = EXTERNAL_PATH
EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("GLM", GLM_INC_PATH, nil, nil)
EXTERNAL_PATH = tmp
  -- BOOST
local BOOST_PATH     = "boost_1_77_0"
local BOOST_LIB_PATH =  path.join(BOOST_PATH, "stage", "lib")  	

EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("BOOST", BOOST_PATH, BOOST_LIB_PATH, nil)
EXTERNAL_PATH = tmp

  -- LUA
local LUA_PATH     = "lua-5.4.2"
local LUA_INC_PATH = path.join(LUA_PATH, "include")
local LUA_LIB_PATH = LUA_PATH

	
EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("LUA", LUA_INC_PATH, LUA_LIB_PATH, "lua54.lib")
EXTERNAL_PATH = tmp


  -- GLFW  
local GLFW_PATH = "glfw"     
local GLFW_INC_PATH = path.join(GLFW_PATH, "include")

local GLFW_LIB_PATH = {
  DEBUG   = path.join(GLFW_PATH, "build", "src", DEBUG),   
  RELEASE = path.join(GLFW_PATH, "build", "src", RELEASE)  
}

EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("GLFW", GLFW_INC_PATH, GLFW_LIB_PATH, "glfw3.lib")
EXTERNAL_PATH = tmp




  -- VISUAL STUDIO REDISTRIBUABLE
  
MSVC_BASELIB_PATH = path.join("MSVC_Redist", "14.23.27820")   
  
MSVC_LIB_PATH_X32_RELEASE = path.join(MSVC_BASELIB_PATH, "x86", "Microsoft.VC142.CRT")
MSVC_LIB_PATH_X64_RELEASE = path.join(MSVC_BASELIB_PATH, "x64", "Microsoft.VC142.CRT") 
MSVC_LIB_PATH_X32_DEBUG = path.join(MSVC_BASELIB_PATH, "debug_nonredist", "x86", "Microsoft.VC142.DebugCRT")
MSVC_LIB_PATH_X64_DEBUG = path.join(MSVC_BASELIB_PATH, "debug_nonredist", "x64", "Microsoft.VC142.DebugCRT") 

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
    "vcruntime140_1.dll"  -- not for x32, but copy should be ignored
}


function GenerateMSVCDLLs(base_path, dlls)

  local result = {}
  for k, v in ipairs(dlls) do
    table.insert(result, "@" .. path.join(base_path, v))
  end
  return result
end

local MSVC_TOCOPY  = { -- @ because this copies the file directly in
  x32 = {
   DEBUG = GenerateMSVCDLLs(MSVC_LIB_PATH_X32_DEBUG, MSVC_DLL_DEBUG),
   RELEASE = GenerateMSVCDLLs(MSVC_LIB_PATH_X32_RELEASE, MSVC_DLL_RELEASE)
  },
  x64 = {
   DEBUG = GenerateMSVCDLLs(MSVC_LIB_PATH_X64_DEBUG, MSVC_DLL_DEBUG),
   RELEASE = GenerateMSVCDLLs(MSVC_LIB_PATH_X64_RELEASE, MSVC_DLL_RELEASE)
  }                  
} 

EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("MSVC", nil, nil, nil, MSVC_TOCOPY)  
EXTERNAL_PATH = tmp

  -- GLEW 
local GLEW_PATH     = "glew-2.2.0"
local GLEW_INC_PATH = path.join(GLEW_PATH, "include")
local GLEW_LIB_PATH = {
  x32 = path.join(GLEW_PATH, "lib", RELEASE, win32),
  x64 = path.join(GLEW_PATH, "lib", RELEASE, x64)
}    
local GLEW_LIBNAME = "glew32s.lib"

local tmp = EXTERNAL_PATH
EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("GLEW", GLEW_INC_PATH, GLEW_LIB_PATH, GLEW_LIBNAME)
EXTERNAL_PATH = tmp



  -- FREETYPE2 

local FREETYPE2_PATH = "freetype2"
local FREETYPE2_INC_PATH = path.join(FREETYPE2_PATH, "include") 
local FREETYPE2_LIB_PATH = {
  x32 = path.join(FREETYPE2_PATH, "objs", win32, "Release Static"),
  x64 = path.join(FREETYPE2_PATH, "objs", x64, "Release Static")
}

local tmp = EXTERNAL_PATH
EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("FREETYPE2", FREETYPE2_INC_PATH, FREETYPE2_LIB_PATH, "freetype.lib")
EXTERNAL_PATH = tmp




  -- IRRKLANG   
local IRRKLANG_PATH = "irrKlang"
local IRRKLANG_INC_PATH = path.join(IRRKLANG_PATH, "include")
local IRRKLANG_LIB_PATH = path.join(IRRKLANG_PATH, "lib", "Winx64-visualStudio")
local IRRKLANG_LIBNAME = "irrKlang.lib"
local IRRKLANG_TOCOPY  = "@" .. path.join(IRRKLANG_PATH, "bin", "winx64-visualStudio" , "irrKlang.dll")  

local tmp = EXTERNAL_PATH
EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("IRRKLANG", IRRKLANG_INC_PATH, IRRKLANG_LIB_PATH, IRRKLANG_LIBNAME, IRRKLANG_TOCOPY)
EXTERNAL_PATH = tmp


  -- FBX 

local FBX_PATH = path.join("FBX SDK", "2019.0") 

local FBX_INC_PATH = path.join(FBX_PATH, "include")  
        
local FBX_X32_PATH = path.join(FBX_PATH, "lib", "vs2015", "x86")
local FBX_X64_PATH = path.join(FBX_PATH, "lib", "vs2015", "x64")

local FBX_LIB_PATH = {
  x32 = {
    DEBUG   = path.join(FBX_X32_PATH, "debug"),   
    RELEASE = path.join(FBX_X32_PATH, "release")
  },
  x64 = {
    DEBUG   = path.join(FBX_X64_PATH, "debug"),   
    RELEASE = path.join(FBX_X64_PATH, "release")
  }
}

local FBX_LIBNAME = "libfbxsdk.lib"

local FBX_TOCOPY  = {   -- @ because this copies the file directly in
  x32 = {
    DEBUG   = "@" .. path.join(FBX_LIB_PATH[x32][DEBUG], "libfbxsdk.dll"),   
    RELEASE = "@" .. path.join(FBX_LIB_PATH[x32][RELEASE], "libfbxsdk.dll"),
  },
  x64 = {
    DEBUG   = "@" .. path.join(FBX_LIB_PATH[x64][DEBUG], "libfbxsdk.dll"),   
    RELEASE = "@" .. path.join(FBX_LIB_PATH[x64][RELEASE], "libfbxsdk.dll"),
  }
}

DeclareExternalLib("FBX", FBX_INC_PATH, FBX_LIB_PATH, FBX_LIBNAME, FBX_TOCOPY)







  -- ZLIB    
local ZLIB_PATH     = "zlib-1.2.11"
local ZLIB_INC_PATH = ZLIB_PATH
local ZLIB_LIB_PATH = path.join(ZLIB_PATH, "contrib",   "vstudio", "vc14", "x64", "ZlibStatRelease")
--local ZLIB_LIB_PATH = path.join(ZLIB_PATH, "contrib",   "vstudio", "vc14", "x64", "ZlibDllRelease")

EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("ZLIB", ZLIB_INC_PATH, ZLIB_LIB_PATH, "zlibstat.lib", nil)     
EXTERNAL_PATH = tmp


    
  -- ASSIMP
local ASSIMP_PATH      = "assimp"
local ASSIMP_INC_PATH  = path.join(ASSIMP_PATH, "include")
local ASSIMP_LIB_PATH  = {
  DEBUG   = path.join(ASSIMP_PATH, "build", "lib", "Debug"),     
  RELEASE = path.join(ASSIMP_PATH, "build", "lib", "Release")            
}
local ASSIMP_LIBNAME  = {
  DEBUG   = "assimp-vc142-mtd.lib",     
  RELEASE = "assimp-vc142-mt.lib"            
}

local ASSIMP_BIN_PATH  = {
  DEBUG   = path.join(ASSIMP_PATH, "build", "bin", "Debug"),     
  RELEASE = path.join(ASSIMP_PATH, "build", "bin", "Release")            
}

local ASSIMP_TOCOPY  = { -- @ because this copies the file directly in
	DEBUG   = "@" .. path.join(ASSIMP_BIN_PATH[DEBUG],   "assimp-vc142-mtd.dll"),     
  RELEASE = "@" .. path.join(ASSIMP_BIN_PATH[RELEASE], "assimp-vc142-mt.dll")            
} 

local tmp = EXTERNAL_PATH
EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("ASSIMP", ASSIMP_INC_PATH, ASSIMP_LIB_PATH, ASSIMP_LIBNAME, ASSIMP_TOCOPY)
EXTERNAL_PATH = tmp


  -- FREEIMAGE
local FREEIMAGE_PATH       = path.join("FreeImage", "Dist")
local FREEIMAGE_DIST_PATH  = {
  x32 = path.join(FREEIMAGE_PATH, "x32"),   
  x64 = path.join(FREEIMAGE_PATH, "x64")                  
}

local FREEIMAGE_INC_PATH = FREEIMAGE_DIST_PATH
local FREEIMAGE_LIB_PATH = FREEIMAGE_DIST_PATH

local FREEIMAGE_LIBNAME = "FreeImage.lib"

local FREEIMAGE_TOCOPY  = { -- @ because this copies the file directly in
  x32 = "@" .. path.join(FREEIMAGE_LIB_PATH[x32], "FreeImage.dll"),
  x64 = "@" .. path.join(FREEIMAGE_LIB_PATH[x64], "FreeImage.dll")                  
} 

EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("FREEIMAGE", FREEIMAGE_INC_PATH, FREEIMAGE_LIB_PATH, FREEIMAGE_LIBNAME, FREEIMAGE_TOCOPY)  
EXTERNAL_PATH = tmp

  -- LIBXML2
local LIBTINYXML2_PATH = "tinyxml2"

local LIBTINYXML2_INC_PATH = LIBTINYXML2_PATH

local LIBTINYXML2_LIB_PATH = path.join(LIBTINYXML2_PATH, "build")

LIBTINYXML2_LIB_PATH = {

	DEBUG = path.join(LIBTINYXML2_LIB_PATH, "Debug"),
	RELEASE = path.join(LIBTINYXML2_LIB_PATH, "Release")
} 

local tmp = EXTERNAL_PATH
EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("TINYXML2", LIBTINYXML2_PATH, LIBTINYXML2_LIB_PATH, "tinyxml2.lib", nil)
EXTERNAL_PATH = tmp


  -- NANA
  
local NANA_PATH     = "nana"
local NANA_INC_PATH = path.join(NANA_PATH, "include")

local NANA_LIB_PATH = path.join(NANA_PATH, "build", "bin")

local NANA_LIBNAME = "nana_v142_Release_x64.lib"

local tmp = EXTERNAL_PATH
EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("NANA", NANA_INC_PATH, NANA_LIB_PATH, NANA_LIBNAME)
EXTERNAL_PATH = tmp







  -- TRISTRIPPER
local TRISTRIPPER_PATH     = "tristripper-master"
local TRISTRIPPER_INC_PATH = path.join(TRISTRIPPER_PATH, "include")
local TRISTRIPPER_LIB_PATH = {
  x32 = {
    DEBUG   = path.join(TRISTRIPPER_PATH, "mysolution", BUILD_TARGET, win32, DEBUG),   
    RELEASE = path.join(TRISTRIPPER_PATH, "mysolution", BUILD_TARGET, win32, RELEASE)
  },
  x64 = {
    DEBUG   = path.join(TRISTRIPPER_PATH, "mysolution", BUILD_TARGET, win64, DEBUG),   
    RELEASE = path.join(TRISTRIPPER_PATH, "mysolution", BUILD_TARGET, win64, RELEASE)  
  },
} 
DeclareExternalLib("TRISTRIPPER", TRISTRIPPER_INC_PATH, TRISTRIPPER_LIB_PATH, "tristripper.lib")

  -- JSON
local JSON_PATH     = "json"
local JSON_INC_PATH = path.join(JSON_PATH, "single_include", "nlohmann")  
local tmp = EXTERNAL_PATH
EXTERNAL_PATH = NEW_EXTERNAL_PATH
DeclareExternalLib("JSON", JSON_INC_PATH, nil, nil)
EXTERNAL_PATH = tmp


  -- VULKAN   
	
	--[[
	
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

  -- GLSLANG   
  
local GLSLANG_PATH = path.join("VulkanSDK", "1.2.162.0")
local GLSLANG_INC_PATH = path.join(GLSLANG_PATH, "Include")
local GLSLANG_LIB_PATH = path.join(GLSLANG_PATH, "Lib")
local GLSLANG_BIN_PATH = path.join(GLSLANG_PATH, "BIN")

local GLSLANG_COMMON_LIBNAME = {"spirv-cross-core.lib", "spirv-cross-cpp.lib", "spirv-cross-glsl.lib"}
local GLSLANG_LIBNAME = 
{
  DEBUG = {"glslangd.lib", "MachineIndependentd.lib", "GenericCodeGend.lib", "OSDependentd.lib", "OGLCompilerd.lib", "HLSLd.lib", "SPIRVd.lib", table.unpack(GLSLANG_COMMON_LIBNAME)},
  RELEASE = {"glslang.lib", "MachineIndependent.lib", "GenericCodeGen.lib", "OSDependent.lib", "OGLCompiler.lib", "HLSL.lib", "SPIRV.lib", table.unpack(GLSLANG_COMMON_LIBNAME)}
}

--local GLSLANG_LIBNAME = {"spirv-cross-core.lib", "spirv-cross-cpp.lib", "spirv-cross-glsl.lib", "glslang.lib", "MachineIndependent.lib", "GenericCodeGen.lib", "shaderc.lib", "OSDependent.lib", "OGLCompiler.lib", "HLSL.lib"}
--local GLSLANG_LIBNAME = {"spirv-cross-c.lib", "spirv-cross-core.lib", "spirv-cross-cpp.lib", "spirv-cross-c-shared.lib", "spirv-cross-glsl.lib", "spirv-cross-hlsl.lib", "spirv-cross-msl.lib", "spirv-cross-reflect.lib", "spirv-cross-util.lib"}

DeclareExternalLib("GLSLANG", GLSLANG_INC_PATH, GLSLANG_LIB_PATH, GLSLANG_LIBNAME)

]]--



