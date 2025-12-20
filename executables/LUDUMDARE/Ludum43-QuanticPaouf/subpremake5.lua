-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum43-QuanticPaouf
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("chaos")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum43PCH.h"),
	path.join("src","Ludum43PCH.cpp")
)