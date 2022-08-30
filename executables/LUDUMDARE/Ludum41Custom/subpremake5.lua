-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum41Custom
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum41CustomPCH.h"),
	path.join("src","Ludum41CustomPCH.cpp")
)