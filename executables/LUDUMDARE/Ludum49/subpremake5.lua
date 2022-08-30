-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum49
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum49PCH.h"),
	path.join("src","Ludum49PCH.cpp")
)