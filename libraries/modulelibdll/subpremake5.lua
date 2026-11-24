-- =============================================================================
-- ROOT_PATH/libraries/modulelibll
-- =============================================================================

local project = build:SharedLibrary()
project:GenerateConfigFile(path.join("include", "modulelibdll", "Config.h"))