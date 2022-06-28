-- =============================================================================
-- ROOT_PATH/executables/MISC/ClientServer
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")
project:AddFileToCopy("testfile.txt")