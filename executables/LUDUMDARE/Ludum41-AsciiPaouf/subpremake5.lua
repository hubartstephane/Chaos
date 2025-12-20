-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum41-AsciiPaouf
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("chaos")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum41PCH.h"),
	path.join("src","Ludum41PCH.cpp")
)