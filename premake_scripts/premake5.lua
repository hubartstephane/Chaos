-- =============================================================================
-- Global variables
-- =============================================================================

BUILD_TARGET         = _ACTION
ROOT_PATH            = path.join(path.getdirectory(_SCRIPT), "..")
SOLUTION_PATH        = path.join(ROOT_PATH, "solutions", BUILD_TARGET)
EXTERNAL_PATH        = path.join(ROOT_PATH, "external")
BUILD_PATH           = path.join(ROOT_PATH, "build", BUILD_TARGET)
BUILD_TOOLS_PATH     = path.join(ROOT_PATH, "build_tools")
ZIP_PATH             = path.join(ROOT_PATH, "zip")
COPY_SCRIPT          = path.join(BUILD_TOOLS_PATH, "mycopy.py")
ZIP_SCRIPT           = path.join(BUILD_TOOLS_PATH, "myzip.py")
DOXYGEN_SCRIPT       = path.join(BUILD_TOOLS_PATH, "mydoxygen.py")
CLEAN_SCRIPT         = path.join(BUILD_TOOLS_PATH, "myclean.py")
CONFIG_TEMPLATE_PATH = path.join(ROOT_PATH, "premake_scripts", "ConfigTemplate.h")

DEBUG     = "DEBUG"
RELEASE   = "RELEASE"
x32       = "x32"
x64       = "x64"
win32     = "win32"
win64     = "win64"
PLATFORMS = {x64} -- no more 32bit support
CONFIGS   = {DEBUG, RELEASE}

DISPLAY_DEPENDENCIES = true

-- =============================================================================
-- requirement
-- =============================================================================

require 'Object'
require 'Utility'
require 'Log'
require 'BuildSystem'
require 'Project'

-- =============================================================================
-- options
-- =============================================================================

newoption {
	trigger = "arch",
	description = "the architecture for the build"
}

-- =============================================================================
-- Fix crashing premake for codelite
-- =============================================================================

-- see https://github.com/premake/premake-core/issues/1299

premake.override(premake.fileconfig, "getconfig", function(oldFn, fcfg, cfg)
	if fcfg.configs ~= nil then
		return fcfg.configs[cfg]
	else
		return nil
	end
end)

-- =============================================================================
-- create the some object
-- =============================================================================

function traceError(err)
	print("Error: ", err)
	print(debug.traceback())
end

local success, errorMsg = xpcall(function()

	build = BuildSystem:new()
	build:DisplayInformation()

	require 'external_premake5' -- declare external libraries

	local sub_directories = {'libraries', 'executables','shared_resources'}
	build:ProcessSubPremake(sub_directories, true) -- create sub groups

	build:CollectDependencies()
	build:MakeSolution()

end, traceError)

if not success then
	print("Failed with error: ", errorMsg)
end
