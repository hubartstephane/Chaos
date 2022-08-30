-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum45
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum45PCH.h"),
	path.join("src","Ludum45PCH.cpp")
)