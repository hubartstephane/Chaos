-- =============================================================================
-- ROOT_PATH/executables/VULKAN/VulkanTest1
-- =============================================================================

local project = build:ConsoleApp()
project:DependOnLib("CHAOS")
project:AddFileToCopy("resources")