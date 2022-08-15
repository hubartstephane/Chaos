-- =============================================================================
-- ROOT_PATH/executables/LUA/LuaBinding
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")