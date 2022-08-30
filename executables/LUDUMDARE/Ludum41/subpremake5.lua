-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum41
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum41PCH.h"),
	path.join("src","Ludum41PCH.cpp")
)