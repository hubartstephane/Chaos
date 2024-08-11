-- =============================================================================
-- ROOT_PATH/executables/linux
-- =============================================================================

for _, v in ipairs({"test1", "test_glm", "test_tinyxml2", "test_json", "test_glfw", "test_freeimage", "test_classes", "test_lua", "test_freetype2", "test_assimp", "test_glslang", "test_vulkan", "test_zlib", "test_boost", "test_irrklang" })   do
	build:ProcessSubPremake(v, true) -- create a sub groups
end
