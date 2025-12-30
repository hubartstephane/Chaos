-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum48_BouldaoufDash
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("chaos")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum48PCH.h"),
	path.join("src","Ludum48PCH.cpp")
)
