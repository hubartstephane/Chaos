-- =============================================================================
-- ROOT_PATH/executables/LUDUM/Ludum40-AskiPaouf
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("chaos")
project:PrecompiledHeader(
	path.join("Ludum40PCH.h"),
	path.join("src","Ludum40PCH.cpp")
)