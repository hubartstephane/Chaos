-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum44
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("CHAOS")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum44PCH.h"),
	path.join("src","Ludum44PCH.cpp")
)