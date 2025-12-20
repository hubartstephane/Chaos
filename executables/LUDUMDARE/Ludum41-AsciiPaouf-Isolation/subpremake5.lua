-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum41-AsciiPaouf-Isolation
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("chaos")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum41IsolationPCH.h"),
	path.join("src","Ludum41IsolationPCH.cpp")
)