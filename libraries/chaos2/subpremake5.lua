-- =============================================================================
-- ROOT_PATH/libraries/chaos2
-- =============================================================================

local project = build:SharedLibrary()

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

project:PrecompiledHeader(
	path.join("chaos2","Chaos2PCH.h"),
	path.join("src","Chaos2PCH.cpp")
)
