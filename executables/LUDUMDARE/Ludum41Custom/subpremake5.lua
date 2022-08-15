-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum41Custom
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")
project:DependOnLib("CommonFonts")