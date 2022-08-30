-- =============================================================================
-- ROOT_PATH/libraries/testdll
-- =============================================================================

local project = build:SharedLibrary()
project:GenerateConfigFile(path.join("include", "testdll", "Config.h"))