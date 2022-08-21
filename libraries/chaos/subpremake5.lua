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
--project:DependOnLib("VULKAN")
--project:DependOnLib("GLSLANG")

project:PrecompiledHeader("chaos/ChaosPCH.h", "ChaosPCH.cpp")
