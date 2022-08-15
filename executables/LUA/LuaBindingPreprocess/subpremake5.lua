-- =============================================================================
-- ROOT_PATH/executables/LUA/LuaBindingPreprocess
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")