-- =============================================================================
-- ROOT_PATH/executables/GLFW/ParticleRefactor
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")