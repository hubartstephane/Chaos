-- =============================================================================
-- ROOT_PATH/executables/C++/C++DLL
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:DependOnLib("TESTDLL")