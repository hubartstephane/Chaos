-- =============================================================================
-- ROOT_PATH/executables
-- =============================================================================

for _, v in ipairs({"test_external_libraries" })   do
	build:ProcessSubPremake(v, true) -- create a sub groups
end
