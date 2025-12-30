-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum41_AsciiPaouf_JujuAndBarb
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("chaos")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum41JujuAndBarbPCH.h"),
	path.join("src","Ludum41JujuAndBarbPCH.cpp")
)