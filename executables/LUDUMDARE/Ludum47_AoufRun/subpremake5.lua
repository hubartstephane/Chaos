-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum47_AoufRun
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("chaos")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum47PCH.h"),
	path.join("src","Ludum47PCH.cpp")
)