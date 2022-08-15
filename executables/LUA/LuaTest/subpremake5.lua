-- =============================================================================
-- ROOT_PATH/executables/LUA/LuaTest
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")
