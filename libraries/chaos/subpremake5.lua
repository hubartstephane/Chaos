-- =============================================================================
-- ROOT_PATH/libraries/chaos
-- =============================================================================
   
  StaticLib()
  DependOnLib("LUA") 
  DependOnLib("GLM")  
  DependOnLib("BOOST")  
  DependOnLib("FREEIMAGE")  
  DependOnLib("TINYXML2")
  DependOnLib("OPENGL")      
  DependOnLib("GLEW")
  DependOnLib("GLFW")            
  DependOnLib("OVR")
  DependOnLib("IRRKLANG")  
  DependOnLib("NANA")  
  DependOnLib("ASSIMP")       
  DependOnLib("FBX")  
  DependOnLib("TRISTRIPPER")  
  DependOnStandardLib("Dbghelp.lib")  -- used on windows for call stack introspection 
