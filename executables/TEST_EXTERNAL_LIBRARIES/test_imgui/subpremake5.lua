-- =============================================================================
-- ROOT_PATH/executables/linux/test_imgui
-- =============================================================================

local project = build:WindowedApp()
project:DependOnLib("GLFW")
project:DependOnLib("OPENGL")
project:DependOnLib("GLEW")
project:DependOnLib("IMGUI")
