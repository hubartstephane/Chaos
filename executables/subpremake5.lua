-- =============================================================================
-- ROOT_PATH/executables
-- =============================================================================

local BASE_GROUP = CURRENT_GROUP

CURRENT_GROUP = BASE_GROUP .. "/GLFW"
ProcessSubPremake("GLFW")

CURRENT_GROUP = BASE_GROUP .. "/LUA"
ProcessSubPremake("LUA")

CURRENT_GROUP = BASE_GROUP .. "/MISC"
ProcessSubPremake("MISC")

CURRENT_GROUP = BASE_GROUP .. "/NANA"
ProcessSubPremake("NANA")

CURRENT_GROUP = BASE_GROUP .. "/MODEL3D"
ProcessSubPremake("MODEL3D")
