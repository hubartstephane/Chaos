-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum47-AoufRun
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("chaos")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum47PCH.h"),
	path.join("src","Ludum47PCH.cpp")
)