-- =============================================================================
-- ROOT_PATH/executables
-- =============================================================================

--for k, v in ipairs({"LUDUMDARE", "GLFW", "MISC", "TOOLS", "LUA", "SOUND", "NANA", "MODEL3D", "VULKAN", "C++"})   do
for k, v in ipairs({"C++", "LUDUMDARE"})   do
	build:ProcessSubPremake(v, true) -- create a sub groups
end