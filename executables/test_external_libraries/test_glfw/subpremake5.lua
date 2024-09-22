-- =============================================================================
-- ROOT_PATH/executables/linux/test_glfw
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("GLFW")
project:DependOnLib("OPENGL")
project:DependOnLib("GLEW")
