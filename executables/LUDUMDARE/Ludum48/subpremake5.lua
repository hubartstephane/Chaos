-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum48
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("CHAOS")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum48PCH.h"),
	path.join("src","Ludum48PCH.cpp")
)
