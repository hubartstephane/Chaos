-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum43
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("CHAOS")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum43PCH.h"),
	path.join("src","Ludum43PCH.cpp")
)