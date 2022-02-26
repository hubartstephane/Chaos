-- =============================================================================
-- ROOT_PATH/executables
-- =============================================================================

local BASE_GROUP = CURRENT_GROUP

CURRENT_GROUP = BASE_GROUP .. "/LUDUMDARE"
ProcessSubPremake("LUDUMDARE")

CURRENT_GROUP = BASE_GROUP .. "/GLFW"
ProcessSubPremake("GLFW")

CURRENT_GROUP = BASE_GROUP .. "/MISC"
ProcessSubPremake("MISC")

CURRENT_GROUP = BASE_GROUP .. "/TOOLS"
ProcessSubPremake("TOOLS")

CURRENT_GROUP = BASE_GROUP .. "/LUA"
ProcessSubPremake("LUA")

CURRENT_GROUP = BASE_GROUP .. "/SOUND"
ProcessSubPremake("SOUND")

CURRENT_GROUP = BASE_GROUP .. "/NANA"
ProcessSubPremake("NANA")

CURRENT_GROUP = BASE_GROUP .. "/MODEL3D"
ProcessSubPremake("MODEL3D")

CURRENT_GROUP = BASE_GROUP .. "/VULKAN"
ProcessSubPremake("VULKAN")

CURRENT_GROUP = BASE_GROUP .. "/C++"
ProcessSubPremake("C++")

