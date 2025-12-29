-- =============================================================================
-- ROOT_PATH/executables
-- =============================================================================

local create_sub_groups = true
build:ProcessSubPremake("Cpp", create_sub_groups)
build:ProcessSubPremake("GLFW", create_sub_groups)
build:ProcessSubPremake("IMGUI", create_sub_groups)
build:ProcessSubPremake("INPUTS", create_sub_groups)
build:ProcessSubPremake("LUA", create_sub_groups)
build:ProcessSubPremake("LUDUMDARE", create_sub_groups)
build:ProcessSubPremake("MISC", create_sub_groups)
build:ProcessSubPremake("SOUND", create_sub_groups)
build:ProcessSubPremake("TEST_EXTERNAL_LIBRARIES", create_sub_groups)
build:ProcessSubPremake("TEXTURE_ARRAY", create_sub_groups)
build:ProcessSubPremake("TOOLS", create_sub_groups)
build:ProcessSubPremake("VULKAN", create_sub_groups)
