-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum43
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")
project:DependOnLib("CommonFonts")