-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum41-AsciiPaouf-JujuAndBarbara
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("chaos")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum41CustomPCH.h"),
	path.join("src","Ludum41CustomPCH.cpp")
)