workspace "ImGUI"
	configurations { "Debug", "Release" }
   
 project "ImGUIOpenGL"
	kind "StaticLib"
	language "C++"
	architecture "x64"
   
	if (os.target() == "windows") then
		targetdir "bin/windows/%{cfg.buildcfg}"   
	end
	if (os.target() == "linux") then
		targetdir "bin/linux/%{cfg.buildcfg}"   
	end
   
	includedirs {"."}
	files { "*.h", "*.cpp" }
	files { "backends/*glfw*.h", "backends/*glfw*.cpp" }
	files { "backends/*opengl3*.h", "backends/*opengl3*.cpp" }

	includedirs {path.join("..", "glfw", "include")}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
