-- =============================================================================
-- ROOT_PATH/libraries/teststaticlib
-- =============================================================================

local project = build:StaticLibrary()
project:GenerateConfigFile(path.join("include", "testdll", "Config.h"))