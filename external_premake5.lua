-- =============================================================================
-- ROOT_PATH/../external
-- =============================================================================

local WINDOWS = (os.target() == "windows")
local LINUX = (os.target() == "linux")

  -- OPENGL
local GL_INC_PATH = "openGL"
DeclareExternalLib("OPENGL", GL_INC_PATH, nil, "OpenGL32")  
                                             
  -- GLM
local GLM_INC_PATH = "glm"
DeclareExternalLib("GLM", GLM_INC_PATH, nil, nil)

  -- BOOST
local BOOST_PATH     = "boost_1_71_0"
local BOOST_LIB_PATH =  {x32 = path.join(BOOST_PATH, "stage", "lib"), x64 = path.join(BOOST_PATH, "stage", "lib")}
DeclareExternalLib("BOOST", BOOST_PATH, BOOST_LIB_PATH, nil)

  -- LUA
if (LINUX) then
	local LUA_INC_PATH = "/usr/include/lua5.3"
	DeclareExternalLib("LUA", LUA_INC_PATH, nil, "lua5.3")
elseif (WINDOWS) then
	local LUA_PATH     = "lua-5.3.0"
	local LUA_INC_PATH = path.join(LUA_PATH, "src")
	local LUA_LIB_PATH = {
	  x32 = {
	    DEBUG   = path.join(LUA_PATH, "build", BUILD_TARGET, x32, DEBUG),   
	    RELEASE = path.join(LUA_PATH, "build", BUILD_TARGET, x32, RELEASE)
	  },
	  x64 = {
	    DEBUG   = path.join(LUA_PATH, "build", BUILD_TARGET, x64, DEBUG),   
	    RELEASE = path.join(LUA_PATH, "build", BUILD_TARGET, x64, RELEASE)  
	  },
	} 
	DeclareExternalLib("LUA", LUA_INC_PATH, LUA_LIB_PATH, "mylualibrary.lib")
end
  -- GLFW  
local GLFW_X32_PATH = "glfw-3.3.bin.WIN32"     
local GLFW_X64_PATH = "glfw-3.3.bin.WIN64"
local GLFW_INC_PATH = {
  x32 = path.join(GLFW_X32_PATH, "include"),
  x64 = path.join(GLFW_X64_PATH, "include")  
}

local GLFW_LIB_PATH

local GLFW_LIB_VCPATH
if (BUILD_TARGET == "vs2019") then 
  GLFW_LIB_VCPATH = "lib-vc2019"      -- beware confusion vS2019 & vC2019 ('c' & 's')
elseif (BUILD_TARGET == "vs2015") then
  GLFW_LIB_VCPATH = "lib-vc2015"
elseif (BUILD_TARGET == "vs2013") then
  GLFW_LIB_VCPATH = "lib-vc2013"
elseif (BUILD_TARGET == "vs2012") then
  GLFW_LIB_VCPATH = "lib-vc2012"
elseif (BUILD_TARGET == "vs2010") then
  GLFW_LIB_VCPATH = "lib-vc2010"
end  

GLFW_LIB_PATH = {
  x32 = path.join(GLFW_X32_PATH, GLFW_LIB_VCPATH),
  x64 = path.join(GLFW_X64_PATH, GLFW_LIB_VCPATH) 
}

DeclareExternalLib("GLFW", GLFW_INC_PATH, GLFW_LIB_PATH, "glfw3.lib")

  -- GLI          
local GLI_PATH = "gli-0.5.1.0"
DeclareExternalLib("GLI", GLI_PATH, nil, nil)

  -- GLEW 
local GLEW_PATH     = "glew-1.12.0"
local GLEW_INC_PATH = path.join(GLEW_PATH, "include")
local GLEW_LIB_PATH = {
  x32 = {
    DEBUG   = path.join(GLEW_PATH, "lib", DEBUG,   win32),   
    RELEASE = path.join(GLEW_PATH, "lib", RELEASE, win32)
  },
  x64 = {
    DEBUG   = path.join(GLEW_PATH, "lib", DEBUG,   x64),   
    RELEASE = path.join(GLEW_PATH, "lib", RELEASE, x64)  
  },
}    
local GLEW_LIBNAME = {
  DEBUG   = "glew32sd.lib",
  RELEASE = "glew32s.lib"  
}  
DeclareExternalLib("GLEW", GLEW_INC_PATH, GLEW_LIB_PATH, GLEW_LIBNAME)


  -- FREETYPE2 

local FREETYPE2_PATH = "freetype-2.6.5"
local FREETYPE2_INC_PATH = path.join(FREETYPE2_PATH, "include") 
local FREETYPE2_LIB_PATH = {
  x32 = path.join(FREETYPE2_PATH, "lib", "vc2015", win32),
  x64 = path.join(FREETYPE2_PATH, "lib", "vc2015", x64)
}
local FREETYPE2_LIBNAME = {
  DEBUG   = "freetyped.lib",
  RELEASE = "freetype.lib"
}      
DeclareExternalLib("FREETYPE2", FREETYPE2_INC_PATH, FREETYPE2_LIB_PATH, FREETYPE2_LIBNAME)

  -- IRRKLANG   
local IRRKLANG_X32_PATH = "irrKlang-1.6.0.WIN32"
local IRRKLANG_X64_PATH = "irrKlang-1.6.0.WIN64"
local IRRKLANG_INC_PATH = {
  x32 = path.join(IRRKLANG_X32_PATH, "include"),
  x64 = path.join(IRRKLANG_X64_PATH, "include")  
}  
local IRRKLANG_LIB_PATH = {
  x32 = path.join(IRRKLANG_X32_PATH, "lib", "Win32-visualStudio"),
  x64 = path.join(IRRKLANG_X64_PATH, "lib", "Winx64-visualStudio")  
}     
local IRRKLANG_LIBNAME = "irrKlang.lib"
local IRRKLANG_TOCOPY  = {   -- @ because this copies the file directly in
  x32 = "@" .. path.join(IRRKLANG_X32_PATH, "bin", "win32-visualStudio", "irrKlang.dll"), 
  x64 = "@" .. path.join(IRRKLANG_X64_PATH, "bin", "winx64-visualStudio" , "irrKlang.dll")  
} 
DeclareExternalLib("IRRKLANG", IRRKLANG_INC_PATH, IRRKLANG_LIB_PATH, IRRKLANG_LIBNAME, IRRKLANG_TOCOPY)

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

  -- FMODSTUDIO
--[[
local FMODSTUDIO_PATH     = path.join("FMOD SoundSystem", "FMOD Studio API Windows", "api")
local FMODSTUDIO_INC_PATH = path.join(FMODSTUDIO_PATH, "lowlevel", "inc")
local FMODSTUDIO_LIB_PATH = path.join(FMODSTUDIO_PATH, "lowlevel", "lib")
local FMODSTUDIO_LIBNAME = {
  x32 = {DEBUG = "fmodL_vc.lib",   RELEASE = "fmod_vc.lib"},
  x64 = {DEBUG = "fmodL64_vc.lib", RELEASE = "fmod64_vc.lib"} 
}

local FMOD_TOCOPY  = { -- @ because this copies the file directly in
  x32 = {
    DEBUG   = "@" .. path.join(FMODSTUDIO_LIB_PATH, "fmodL.dll"),     
    RELEASE = "@" .. path.join(FMODSTUDIO_LIB_PATH, "fmod.dll"),
  },
  x64 = {
    DEBUG   = "@" .. path.join(FMODSTUDIO_LIB_PATH, "fmodL64.dll"),     
    RELEASE = "@" .. path.join(FMODSTUDIO_LIB_PATH, "fmod64.dll"),
  }                  
} 

DeclareExternalLib("FMODSTUDIO", FMODSTUDIO_INC_PATH, FMODSTUDIO_LIB_PATH, FMODSTUDIO_LIBNAME, FMOD_TOCOPY)    
--]]




  -- ZLIB    
local ZLIB_PATH     = "zlib-1.2.8"
local ZLIB_LIB_PATH  = {
  x32 = {
    DEBUG   = path.join(ZLIB_PATH, "Debug",   "x32"),     
    RELEASE = path.join(ZLIB_PATH, "Release", "x32"),
  },
  x64 = {
    DEBUG   = path.join(ZLIB_PATH, "Debug",   "x64"),     
    RELEASE = path.join(ZLIB_PATH, "Release", "x64"),
  }                  
}
local ZLIB_LIBNAME = {
  DEBUG   = "zlibstaticd.lib",
  RELEASE = "zlibstatic.lib"  
}

DeclareExternalLib("ZLIB", ZLIB_PATH, ZLIB_LIB_PATH, ZLIB_LIBNAME, nil)     




    
  -- ASSIMP
local ASSIMP_PATH      = "assimp-3.1.1"
local ASSIMP_INC_PATH  = path.join(ASSIMP_PATH, "include")
local ASSIMP_LIB_PATH  = {
  x32 = {
    DEBUG   = path.join(ASSIMP_PATH, "lib32", "Debug"),     
    RELEASE = path.join(ASSIMP_PATH, "lib32", "Release"),
  },
  x64 = {
    DEBUG   = path.join(ASSIMP_PATH, "lib64", "Debug"),     
    RELEASE = path.join(ASSIMP_PATH, "lib64", "Release"),
  }                  
}
local ASSIMP_LIBNAME = {
  DEBUG   = "assimpd.lib",
  RELEASE = "assimp.lib"  
}

local ASSIMP_TOCOPY  = { -- @ because this copies the file directly in
  x32 = {
    DEBUG   = "@" .. path.join(ASSIMP_LIB_PATH[x32][DEBUG],   "assimpd.dll"),     
    RELEASE = "@" .. path.join(ASSIMP_LIB_PATH[x32][RELEASE], "assimp.dll"),
  },
  x64 = {
    DEBUG   = "@" .. path.join(ASSIMP_LIB_PATH[x64][DEBUG],   "assimpd.dll"),     
    RELEASE = "@" .. path.join(ASSIMP_LIB_PATH[x64][RELEASE], "assimp.dll"),
  }                  
} 

DeclareExternalLib("ASSIMP", ASSIMP_INC_PATH, ASSIMP_LIB_PATH, ASSIMP_LIBNAME, ASSIMP_TOCOPY)


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

DeclareExternalLib("FREEIMAGE", FREEIMAGE_INC_PATH, FREEIMAGE_LIB_PATH, FREEIMAGE_LIBNAME, FREEIMAGE_TOCOPY)  

  -- LIBXML2
local LIBTINYXML2_PATH = "tinyxml2-master"

local LIBTINYXML2_INC_PATH = LIBTINYXML2_PATH

local LIBTINYXML2_LIB_PATH = path.join(LIBTINYXML2_PATH, "tinyxml2", "bin")

LIBTINYXML2_LIB_PATH = {
  x32 = {DEBUG = path.join(LIBTINYXML2_LIB_PATH, "win32-Debug-Lib"), RELEASE = path.join(LIBTINYXML2_LIB_PATH, "win32-Release-Lib")},
  x64 = {DEBUG = path.join(LIBTINYXML2_LIB_PATH, "x64-Debug-Lib"),   RELEASE = path.join(LIBTINYXML2_LIB_PATH, "x64-Release-Lib")}
} 

DeclareExternalLib("TINYXML2", LIBTINYXML2_PATH, LIBTINYXML2_LIB_PATH, "tinyxml2.lib", nil)

  -- NANA
  
local NANA_PATH     = "nana"
local NANA_INC_PATH = path.join(NANA_PATH, "include")

local NANA_LIB_PATH = path.join(NANA_PATH, "build", "bin")

local NANA_LIBNAME = {
  x32 = {
    DEBUG   = "nana_v140_Debug_x86.lib",   
    RELEASE = "nana_v140_Release_x86.lib"  
  },
  x64 = {
    DEBUG   = "nana_v140_Debug_x64.lib",     
    RELEASE = "nana_v140_Release_x64.lib"  
  }
} 
DeclareExternalLib("NANA", NANA_INC_PATH, NANA_LIB_PATH, NANA_LIBNAME)







   -- OCULUS SDK (OVR)
--[[
local OVR_PATH = "OculusSDK"

local OVR_INC_PATH = path.join(OVR_PATH, "LibOVR", "Include")

local OVR_LIB_PATH = path.join(OVR_PATH, "LibOVR", "Lib", "Windows")

OVR_LIB_PATH = {
  x32 = path.join(OVR_LIB_PATH, "Win32", "Release", BUILD_TARGET),
  x64 = path.join(OVR_LIB_PATH, "x64", "Release", BUILD_TARGET),
} 

DeclareExternalLib("OVR", OVR_INC_PATH, OVR_LIB_PATH, "LibOVR.lib", nil)
--]]





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
local JSON_PATH     = "json-develop"
local JSON_INC_PATH = path.join(JSON_PATH, "src")  

DeclareExternalLib("JSON", JSON_INC_PATH, nil, nil)






  -- FFMPEG
--[[
local FFMPEG_PATH     = "ffmpeg"
local FFMPEG_INC_PATH = {
  x32 = path.join(FFMPEG_PATH, "win32", "include"),
  x64 = path.join(FFMPEG_PATH, "win64", "include")
} 
local FFMPEG_LIB_PATH = {
  x32 = path.join(FFMPEG_PATH, "win32", "lib"),
  x64 = path.join(FFMPEG_PATH, "win64", "lib")
} 
--DeclareExternalLib("FFMPEG", FFMPEG_INC_PATH, FFMPEG_LIB_PATH, {"avcodec.lib", "avdevice.lib", "avfilter.lib", "avformat.lib", "avutil.lib", "postproc.lib", "swresample.lib", "swscale.lib"}) 
--]]






  -- OPENCV  
  
  --[[
  
local OPENCV_PATH     = "opencv"
local OPENCV_INC_PATH = path.join(OPENCV_PATH, "build", "include")
local OPENCV_LIB_PATH = {
  x32 = {
    DEBUG   = path.join(OPENCV_PATH, "build", "win32", "vc15", "lib"),   
    RELEASE = path.join(OPENCV_PATH, "build", "win32", "vc15", "lib")  
  },
  x64 = {
    DEBUG   = path.join(OPENCV_PATH, "build", "x64", "vc15", "lib"),   
    RELEASE = path.join(OPENCV_PATH, "build", "x64", "vc15", "lib")  
  },
} 

local OPENCV_LIBNAME = {
  DEBUG   = "opencv_world400d.lib",
  RELEASE = "opencv_world400.lib"  
}
local OPENCV_TOCOPY  = {   -- @ because this copies the file directly in
  x32 = {
    DEBUG   = "@" .. path.join(OPENCV_PATH, "build", "x32", "vc15", "bin", "opencv_world400d.dll"), 
    RELEASE = "@" .. path.join(OPENCV_PATH, "build", "x32", "vc15", "bin", "opencv_world400.dll"), 
  },
  x64 = {
    DEBUG   = "@" .. path.join(OPENCV_PATH, "build", "x64", "vc15", "bin", "opencv_world400d.dll"), 
    RELEASE = "@" .. path.join(OPENCV_PATH, "build", "x64", "vc15", "bin", "opencv_world400.dll"), 
  }
} 
DeclareExternalLib("OPENCV", OPENCV_INC_PATH, OPENCV_LIB_PATH, OPENCV_LIBNAME, OPENCV_TOCOPY)

--]]  
