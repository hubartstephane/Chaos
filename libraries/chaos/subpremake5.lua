-- =============================================================================
-- ROOT_PATH/libraries/chaos
-- =============================================================================
   
  StaticLib()

  DependOnLib("RANGE-V3") 
  DependOnLib("LUA") 
  DependOnLib("GLM")  
  DependOnLib("BOOST")  
  --DependOnLib("VULKAN")  
  --DependOnLib("GLSLANG")  
  DependOnLib("FREEIMAGE")  
  DependOnLib("TINYXML2")
  DependOnLib("OPENGL")      
  DependOnLib("GLEW")
  DependOnLib("GLFW")            
  DependOnLib("IRRKLANG")  
  DependOnLib("NANA")  
  DependOnLib("ASSIMP")       
  DependOnLib("FBX")  
  DependOnLib("TRISTRIPPER")
  DependOnLib("FREETYPE2")
  DependOnLib("JSON")
  DependOnLib("ZLIB")
  DependOnLib("MSVC")

  DependOnStandardLib("Dbghelp.lib")  -- used on windows for call stack introspection 
  DependOnStandardLib("winmm.lib")    -- used for midi

  -- pchheader "StandardHeaders.h"
   --pchsource "StandardHeaders.cpp"