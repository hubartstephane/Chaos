-- =============================================================================
-- ROOT_PATH/executables/MISC/SkyBoxLoading
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")
