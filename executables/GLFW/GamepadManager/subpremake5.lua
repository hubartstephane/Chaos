-- =============================================================================
-- ROOT_PATH/executables/GLFW/GamepadManager
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")