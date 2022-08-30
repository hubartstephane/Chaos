-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum41Isolation
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:DependOnLib("CommonFonts")
project:PrecompiledHeader(
	path.join("Ludum41IsolationPCH.h"),
	path.join("src","Ludum41IsolationPCH.cpp")
)