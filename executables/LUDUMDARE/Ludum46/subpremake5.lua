-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum46
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("chaos")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum46PCH.h"),
	path.join("src","Ludum46PCH.cpp")
)
