-- =============================================================================
-- ROOT_PATH/executables/TEST_EXTERNAL_LIBRARIES/test_glfw
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("GLFW")
project:DependOnLib("OPENGL")
project:DependOnLib("GLEW")
