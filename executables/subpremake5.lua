-- =============================================================================
-- ROOT_PATH/executables
-- =============================================================================

for _, v in ipairs({"linux" })   do
	build:ProcessSubPremake(v, true) -- create a sub groups
end
