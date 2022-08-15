-- =============================================================================
-- ROOT_PATH/executables/GLFW/Spriter
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")