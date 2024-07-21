-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum47
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("CHAOS")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum47PCH.h"),
	path.join("src","Ludum47PCH.cpp")
)