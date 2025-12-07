-- =============================================================================
-- ROOT_PATH/executables/MISC/ClientServer
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("chaos")
project:AddFileToCopy("testfile.txt")