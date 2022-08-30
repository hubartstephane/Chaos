-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum50
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum50PCH.h"),
	path.join("src","Ludum50PCH.cpp")
)