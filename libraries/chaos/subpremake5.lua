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
--project:DependOnLib("NANA")
project:DependOnLib("ASSIMP")
project:DependOnLib("FBX")

--project:DependOnLib("RANGE-V3")
--project:DependOnLib("VULKAN")
--project:DependOnLib("GLSLANG")

project:DependOnStandardLib("Dbghelp.lib")  -- used on windows for call stack introspection 
project:DependOnStandardLib("winmm.lib")    -- used for midi
-- pchheader "StandardHeaders.h"
--pchsource "StandardHeaders.cpp"