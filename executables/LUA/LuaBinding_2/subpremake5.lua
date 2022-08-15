-- =============================================================================
-- ROOT_PATH/executables/LUA/LuaBinding_2
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")