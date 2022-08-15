-- =============================================================================
-- ROOT_PATH/executables/MISC/MyZLib
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")