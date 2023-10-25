===================================================================================================================
Freetype
========

solution is in 

  freetype2\builds\windows\vc2010

->build "Release Static x64" with Visual studio

->the library is 

	freetype2\objs\x64\Release Static\freetype.lib
	
===================================================================================================================	
GLEW
====

  failed to compile this (sources files are in 'auto' directory instead of 'src')
	Use .zip found in 
	  https://github.com/nigels-com/glew
		
		https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
		
	(instead of having a fork or clone)

  I even tryed to use cmake with no success. Source project seems corrupted
	
===================================================================================================================	
TINYXML2
========

  solution in tinyxml2/vs are incorrect (build an executable and no library)
	recreate solution
	
	  cmake -S . -B projects  -> generate solution in build

===================================================================================================================
BOOST
=====

  boost has submodules inside. I forked boost but i should probably fork its submodules too. Maybe it isn't even the good solution
  so i use a ZIP download
	
	to build. From Visual studio start a console
	bootstrap msvc
	b2 toolset=msvc library=static
	
===================================================================================================================	
ASSIMP
======

  see https://github.com/assimp/assimp/blob/master/Build.md
	
	cmake CMakeLists.txt 
	cmake --build .
	
===================================================================================================================	
Zlib
====

  there is the solution zlib-1.2.11\contrib\vstudio\vc14
	build zlibstat for static library

===================================================================================================================
GLFW
====
	--> telecharger la build DLL windows directement c'est moins compliqué d'avoir header+dll 


#if 0
	telecharger les sources de glfw dans un repertoire path/to/glfw
	on va creer une solution .sln dans un repertoire completement different grace a cmake

	cmake -S path/to/glfw -B path/to/build -D BUILD_SHARED_LIBS=ON
	
	https://www.glfw.org/docs/3.3/compile.html
#endif	
	
===================================================================================================================
GLSLANG
=======

	suivre

    https://github.com/KhronosGroup/glslang
	
	(attention, modifier le shebang de update_glslang_sources.py pour trouver python)

	git clone https://github.com/KhronosGroup/glslang.git
	
	(attention, le faut installer DEBUG et RELEASE ... parcequ'on ne peut pas linker CHAOS DEBUG avec GLSLANG RELEASE)
	
===================================================================================================================
IMGUI
=====

	by default imgui is a set of c++ files with mixed backends (vulkan, openGL ..)
	There is no real library provided. You rather have to directly insert thoses files into your own project 
	(with only the backends you are interrested in)
	I rather created my own ImGUI.sln with the following premake and so was able to have a static library
	
		
workspace "ImGUI"
   configurations { "Debug", "Release" }
   
 project "ImGUIOpenGL"
   kind "StaticLib"
   language "C++"
   architecture "x64"
   
   targetdir "bin/%{cfg.buildcfg}"
   
   includedirs {"."}
   files { "*.h", "*.cpp" }
   files { "backends/*glfw*.h", "backends/*glfw*.cpp" }
   files { "backends/*opengl3*.h", "backends/*opengl3*.cpp" }

   includedirs {path.join("..", "glfw-3.3.8", "include")}

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
	  
	  