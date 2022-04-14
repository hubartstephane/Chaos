-- =============================================================================
-- Generic function
-- =============================================================================

function LuaTarget(project_name, target_kind, excluded_files)

  project (project_name)      
		kind (target_kind)

		language "C++"
		files {"src/**.h", "src/**.c", "src/**.hpp" }
		excludes (excluded_files)
		defines("_CRT_SECURE_NO_WARNINGS")
		
		
		--[[
		if (os.target() == "windows") then		
			defines("LUA_USE_WINDOWS")		
		else
			defines("LUA_USE_LINUX")		
		end
		]]--
		
	filter {"configurations:DEBUG", "platforms:x64"}
    defines { "DEBUG" }
    defines { "_DEBUG" }
    symbols "On"   		
		optimize "Off"
		targetdir ("build/" .._ACTION .. "/x64/debug")
      
	filter {"configurations:RELEASE", "platforms:x64"}    
    defines { "RELEASE" }
    defines { "_RELEASE" }
		defines { "NDEBUG" }
		symbols "Off"   		
    optimize "On"    			
		targetdir ("build/" .._ACTION .. "/x64/release")    

end

-- =============================================================================
-- Main description
-- =============================================================================

solution "Lua Library"
  platforms { "x64" }
  configurations { "DEBUG" , "RELEASE" }
  location ("premaked_solutions/" .. _ACTION)  
        
	LuaTarget("staticlualib", "StaticLib", {"src/lua.c", "src/luac.c"})
	LuaTarget("sharedlualib", "SharedLib", {"src/lua.c", "src/luac.c"})
	LuaTarget("lua", "ConsoleApp", {"src/luac.c"})
	LuaTarget("luac", "ConsoleApp", {"src/lua.c"})

    
