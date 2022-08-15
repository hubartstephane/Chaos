-- =============================================================================
-- ROOT_PATH/executables/GLFW/ViewportWindow
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")
