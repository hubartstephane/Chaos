Freetype
========

solution is in 

  freetype2\builds\windows\vc2010

->build "Release Static x64" with Visual studio

->the library is 

	freetype2\objs\x64\Release Static\freetype.lib
	
GLEW
====

  failed to compile this (sources files are in 'auto' directory instead of 'src')
	Use .zip found in 
	  https://github.com/nigels-com/glew
		
		https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
		
	(instead of having a fork or clone)

  I even tryed to use cmake with no success. Source project seems corrupted
	
TINYXML2
========

  solution in tinyxml2/vs are incorrect (build an executable and no library)
	recreate solution
	
	  cmake -S . -B projects  -> generate solution in build

BOOST
=====

  boost has submodules inside. I forked boost but i should probably fork its submodules too. Maybe it isn't even the good solution
  so i use a ZIP download
	
	to build. From Visual studio start a console
	bootstrap msvc
	b2 toolset=msvc library=static
	
	
ASSIMP
======

  see https://github.com/assimp/assimp/blob/master/Build.md
	
	cmake CMakeLists.txt 
	cmake --build .
	
Zlib
====

  there is the solution zlib-1.2.11\contrib\vstudio\vc14
	build zlibstat for static library
	
NANA
====

  solution is here ./build/vc2019/nana.sln
	
	then the lib is here ./build/bin/nana_v142_Release_x64.lib
	
	beware, a temp directory is created in external

GLFW
====

	cmake .
	