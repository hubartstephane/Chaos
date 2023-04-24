-- =============================================================================
-- ROOT_PATH/libraries/chaos
-- =============================================================================

local project = build:StaticLibrary()

project:DependOnLib("LUA")
project:DependOnLib("GLM")
project:DependOnLib("BOOST")
project:DependOnLib("FREEIMAGE")
project:DependOnLib("TINYXML2")
project:DependOnLib("OPENGL")
project:DependOnLib("GLEW")
project:DependOnLib("GLFW")
project:DependOnLib("IRRKLANG")
project:DependOnLib("FREETYPE2")
project:DependOnLib("JSON")
project:DependOnLib("ZLIB")
project:DependOnLib("MSVC")
project:DependOnLib("ASSIMP")
project:DependOnLib("FBX")
project:DependOnLib("GLSLANG")
project:DependOnLib("VULKAN")
project:DependOnLib("IMGUI")

project:DependOnStandardLib("Dbghelp.lib")  -- used on windows for call stack introspection 
project:DependOnStandardLib("winmm.lib")    -- used for midi

project:PrecompiledHeader(
	path.join("chaos","ChaosPCH.h"),
	path.join("src","ChaosPCH.cpp")
)

project:GenerateConfigFile(path.join("include", "chaos", "Config.h"))
