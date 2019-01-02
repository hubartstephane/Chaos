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
  DependOnLib("IRRKLANG")  
  DependOnLib("NANA")  
  DependOnLib("ASSIMP")       
  DependOnLib("FBX")  
  DependOnLib("TRISTRIPPER")
  DependOnLib("FREETYPE2")
  DependOnLib("JSON")
  DependOnLib("ZLIB")
  
  --DependOnLib("OVR")  
  --DependOnLib("OPENCV")
  --DependOnLib("FFMPEG")                  
  --DependOnLib("FMODSTUDIO")  
  --DependOnLib("GLI")  
  --DependOnLib("LIBXML2")    
     
  DependOnStandardLib("Dbghelp.lib")  -- used on windows for call stack introspection 
  DependOnStandardLib("winmm.lib")    -- used for midi
