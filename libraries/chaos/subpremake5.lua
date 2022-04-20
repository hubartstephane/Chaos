-- =============================================================================
-- ROOT_PATH/libraries/chaos
-- =============================================================================

  StaticLibrary()

  DependOnLib("LUA")
  DependOnLib("GLM")
  DependOnLib("BOOST")
  DependOnLib("FREEIMAGE")
  DependOnLib("TINYXML2")
  DependOnLib("OPENGL")
  DependOnLib("GLEW")
  DependOnLib("GLFW")        
  DependOnLib("IRRKLANG")
  DependOnLib("FREETYPE2")
  DependOnLib("JSON")
  DependOnLib("ZLIB")
  DependOnLib("MSVC")
	
  DependOnLib("NANA")
  DependOnLib("ASSIMP")
  DependOnLib("FBX")
  --DependOnLib("TRISTRIPPER")
  DependOnLib("RANGE-V3")
  --DependOnLib("VULKAN")
  --DependOnLib("GLSLANG")

  DependOnStandardLib("Dbghelp.lib")  -- used on windows for call stack introspection 
  DependOnStandardLib("winmm.lib")    -- used for midi

  -- pchheader "StandardHeaders.h"
   --pchsource "StandardHeaders.cpp"