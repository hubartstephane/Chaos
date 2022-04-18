-- =============================================================================
-- ROOT_PATH/executables
-- =============================================================================

local BASE_GROUP = CURRENT_GROUP

for k, v in ipairs({"LUDUMDARE", "GLFW", "MISC", "TOOLS", "LUA", "SOUND", "NANA", "MODEL3D", "VULKAN", "C++"})   do

	CURRENT_GROUP = BASE_GROUP .. "/" .. v
	ProcessSubPremake(v)
	
end


