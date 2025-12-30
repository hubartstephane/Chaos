-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum44_KillPaouf
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("chaos")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum44PCH.h"),
	path.join("src","Ludum44PCH.cpp")
)