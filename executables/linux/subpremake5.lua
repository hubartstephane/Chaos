-- =============================================================================
-- ROOT_PATH/executables/linux
-- =============================================================================

for _, v in ipairs({"test1" })   do
	build:ProcessSubPremake(v, true) -- create a sub groups
end
