-- =============================================================================
-- ROOT_PATH/executables/GLFW/CubeMap
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")