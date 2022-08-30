-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum40
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:PrecompiledHeader(
	path.join("Ludum40PCH.h"),
	path.join("src","Ludum40PCH.cpp")
)