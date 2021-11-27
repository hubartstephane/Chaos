Generic considerations
======================

The following project is composed of:  
	
* the **chaos** library (the game library)
* some tests executables
* some games for ludum dare game jam

It is being build with **premake5** for Visual Studio 2019

It is coded in C++ and rely on some external librairies including (but not only):

* boost
* irrklang
* openGL
* glm
* tinyxml2
* glfw
* freetype2
* freeimage
* glew
* nlohmann


To generate the Visual Studio solution you just have to get in the project directory and execute

	generate.bat

The root directory of this project contain the following elements:

name| comment
----|--------
executables|  the directory where executables projects are
libraries|  the directory where libraries projects are
shared_resources|  the directory where resources only projects are
documentation| where you have found this documentation/
build| where projects are beeing build (executable or doxygen documentation)
solutions| where **premake5** generate the visual studio solution
zip| where zip project are saved
premake5.lua| the main file used by premake5
external_premake5.lua| the file that describes for premake5 all external dependancies (libraries, include path, link path ...)
generate.bat| execute premake5 with parameters







The main premake file is named **premake5.lua**. It contains some important functions and start to generate projects recursively with the following lua function (the subdirectory has to contain a file named **subpremake5.lua**)  

	ProcessSubPremake("directory_name")
	
In lua files you may use the following function to create a library

	StaticLib()
	
In lua files you may use the following function to create an executables/

	WindowedApp()
 
Any of thoses kind of projects may declare dependancies with
 
	DependOnLib("CHAOS")



Subdirectories may contain files named **subpremake5.lua** to declare other project to build
If you open the root of the project you will find the following files/directories






The root of the project contains a file named **premake5.lua** and some directories contain
To build the solution you just have to call **generate.bat**


Reading JSON recursively
========================

You may read JSON recursively with **JSONRecursiveLoader**  
In your JSON file, any object containing a property named **@include** is replaced by the content of the JSON file whose path is following

resources/config.json:

	{
	  "gpu":{"@include":"@gpu/gpu.json"}
	}  

resources/gpu/gpu.json:
	
	[
		"my_texture1":"@texture1.png"
		"my_texture2":"@texture2.png"
	]

After substitution (with the file **@gpu/gpu.json**), we would have the following final JSON content

	{
	  "gpu":[
			"my_texture1":"resources/gpu/texture1.png",
			"my_texture2":"resources/gpu/texture2.png",
		]
	}  

 The @ character at the beginning of a property value is replaced by the path of the JSON file beeing considered. So you can keep using relative path inside your JSON files:
 *  **@gpu/gpu.json** is so replaced with **resources**/gpu/gpu.json
 *  **@texture1.png** is so replaced with **resources/gpu**/texture1.png



In this example, the name of each resources is given by the member **name**

	[
		{"name":"background", "path": "@programs/background/background.pgm" },
		{"name":"text", "path": "@programs/text/text.pgm" },
		{"name":"screenspace1", "path": "@programs/screenspace1/screenspace1.pgm" },
		{"name":"screenspace2", "path": "@programs/screenspace2/screenspace2.pgm" },
		{"name":"BurningFlames", "path": "@programs/BurningFlames/BurningFlames.pgm" },
		{"name":"blackscreen", "path": "@programs/blackscreen/blackscreen.pgm" }
	]

In this example, the name of the each resources is the key for each line (because it begins with **@**)

	{
		"@background" : { "path": "@programs/background/background.pgm" },
		"@player": { "path": "@programs/player/player.pgm" },
		"@enemy": { "path": "@programs/enemy/enemy.pgm" },
		"@health": { "path": "@programs/health/health.pgm" },
		"@fire": { "path": "@programs/fire/fire.pgm" },
		"@explosion": { "path": "@programs/explosion/explosion.pgm" },
		"@power_up": { "path": "@programs/power_up/power_up.pgm" },  
		"@text"  : { "path": "@programs/text/text.pgm" }    
	}
