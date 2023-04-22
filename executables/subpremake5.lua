-- =============================================================================
-- ROOT_PATH/executables
-- =============================================================================

for _, v in ipairs({"LUDUMDARE", "GLFW", "MISC", "TOOLS", "LUA", "SOUND", "NANA", "MODEL3D", "VULKAN", "C++", "IMGUI"})   do
	build:ProcessSubPremake(v, true) -- create a sub groups
end