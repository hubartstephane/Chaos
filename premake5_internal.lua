-- =============================================================================
-- Encode a string into a base 64 (code coming from internet)
-- =============================================================================

function Base64Encode(data)
	local b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

	return ((data:gsub(
		".",
		function(x)
			local r, b = "", x:byte()
			for i = 8, 1, -1 do
				r = r .. (b % 2 ^ i - b % 2 ^ (i - 1) > 0 and "1" or "0")
			end
			return r
		end
		) .. "0000"):gsub(
			"%d%d%d?%d?%d?%d?",
			function(x)
				if (#x < 6) then
					return ""
				end
				local c = 0
				for i = 1, 6 do
					c = c + (x:sub(i, i) == "1" and 2 ^ (6 - i) or 0)
				end
				return b:sub(c + 1, c + 1)
			end
		) .. ({"", "==", "="})[#data % 3 + 1])
end

-- =============================================================================
-- Type functions
-- =============================================================================

function IsBool(value)
	return (type(value) == "boolean")
end

function IsTable(value)
	return (type(value) == "table")
end

function IsNil(value)
	return (type(value) == "nil")
end

function IsString(value)
	return (type(value) == "string")
end

function IsNumber(value)
	return (type(value) == "number")
end

-- =============================================================================
-- Read an entry from an array
-- =============================================================================

-- returns true whether value is a table with PLATFORM/CONFIGS key
function HasCategoryKey(src, categories)
	if (not IsTable(src)) then
		return false
	end

	for k, category in pairs(categories) do
		if (not IsNil(src[category])) then
			return true
		end
	end
	return false
end

-- returns true whether value is a table with PLATFORM key
function HasPlatformKey(src)
	return HasCategoryKey(src, PLATFORMS)
end

-- returns true whether value is a table with CONFIG key
function HasConfigKey(src)
	return HasCategoryKey(src, CONFIGS)
end

-- if input is an array, returns a copy of it. return the input unchanged elsewhere
function DeepCopy(value)
	if (IsTable(value)) then
			return table.deepcopy(value)
	end
	return value
end

-- if the input is not an array return it unchanged
-- elsewhere, read in that array the value corresponding to PLATFORM/CONFIG
function ReadValueFromPlatformConfigArray(value, plat, conf)
	if (HasPlatformKey(value)) then
		if (HasConfigKey(value[plat])) then
			return value[plat][conf]
		end
		return value[plat]
	elseif (HasConfigKey(value)) then
		if (HasPlatformKey(value[conf])) then
			return value[conf][plat]
		end
		return value[conf]
	end
	return value
end

-- transform a value (array or not) in the form of [PLATFORM][CONFIG]
function GetPlatConfArray(value)
	local result = {}
	for k, platform in pairs(PLATFORMS) do
		result[platform] = {}
		for k, config in pairs(CONFIGS) do
			result[platform][config] = DeepCopy(ReadValueFromPlatformConfigArray(value, platform, config))
		end
	end
	return result
end

-- =============================================================================
-- call fun with all combinaison of PLATFORM & CONFIG
-- =============================================================================

function AllTargets(fun)
	for k, platform in pairs(PLATFORMS) do
		for k, config in pairs(CONFIGS) do
			fun(platform, config)
		end
	end
end

function ForEachElement(src, fun)
  if not IsNil(src) then
    if IsTable(src) then
	    for k, v in pairs(src) do
		    fun(v)
		  end
	  else
	    fun(src)
		end
	end
end

-- =============================================================================
-- String concatenation
-- =============================================================================

function QuotationMarks(...)
	local arg = {...}

	local result = ""
	for k, v in pairs(arg) do
		if (result ~= "") then
			result = result .. " "
		end
		result = result .. '"' .. tostring(v) .. '"'
	end
	return result
end

-- =============================================================================
-- Functions to display a line
-- =============================================================================

function GetIndentText(indent)
	local txt = ""
	for i = 1, indent do
		txt = "  " .. txt
	end
	return txt
end

function Output(txt)
	local ident_txt = GetIndentText(INDENT)
	local lines = string.explode(txt, "\n")
	for k, line in pairs(lines) do
		print(ident_txt .. line)
	end
end

-- =============================================================================
-- Get a string representation of an object
-- =============================================================================

function GetDebugRepresentationString(obj)
	if (IsBool(obj)) then
		if (obj) then
			return "true"
		else
			return "false"
		end
	end
	if (IsNil(obj)) then
		return "nil"
	end
	if (IsNumber(obj)) then
		return tostring(obj)
	end
	if (IsString(obj)) then
		return '"' .. obj .. '"'
	end
	if (IsTable(obj)) then
		local result = "{"
		local first = true
		for k, v in pairs(obj) do
			if (not first) then
				result = result .. ", "
			end
			result = result .. GetDebugRepresentationString(k) .. " => " .. GetDebugRepresentationString(v)
			first = false
		end
		result = result .. "}"
		return result
	end
	return "???"
end

-- =============================================================================
-- Functions to display the environment
-- =============================================================================

function DisplayRootEnvironment()
	if (DISPLAY_ROOT_ENVIRONMENT) then
		Output("=======================================================================")
		Output("PREMAKE5 VERSION   : " .. _PREMAKE_VERSION)
		Output("BUILD_TARGET       : " .. BUILD_TARGET)
		Output("ROOT_PATH          : " .. ROOT_PATH)
		Output("ZIP_PATH           : " .. ZIP_PATH)
		Output("EXTERNAL_PATH      : " .. EXTERNAL_PATH)
		Output("BUILD_TOOLS_PATH   : " .. BUILD_TOOLS_PATH)
		Output("COPY_SCRIPT        : " .. COPY_SCRIPT)
		Output("ZIP_SCRIPT         : " .. ZIP_SCRIPT)
		Output("DOXYGEN_SCRIPT     : " .. DOXYGEN_SCRIPT)
		Output("CLEAN_SCRIPT       : " .. CLEAN_SCRIPT)
		Output("=======================================================================")
		Output("")
	end
end

function DisplayEnvironment()
	if (DISPLAY_ENVIRONMENT) then
		Output("=======================================================================")
		Output("SCRIPT             : " .. _SCRIPT)
		Output("PROJ_NAME          : " .. PROJ_NAME)
		Output("PROJECT_PATH       : " .. PROJECT_PATH)
		Output("PROJECT_SRC_PATH   : " .. PROJECT_SRC_PATH)
		Output("PROJECT_BUILD_PATH : " .. PROJECT_BUILD_PATH)
		Output("=======================================================================")
		Output("")
	end
end

-- =============================================================================
-- Search into an array
-- =============================================================================

function SearchElementInTable(src, elem)
	for k, v in pairs(src) do
		if (v == elem) then
			return true
		end
	end
	return false
end

-- =============================================================================
-- Dependant project names
-- =============================================================================

function GetDependantDocumentationProjName(proj_name)
	return "Documentation_of_" .. proj_name
end

function GetDependantZipProjName(proj_name)
	return "ZIP_of_" .. proj_name
end

function GetDependantResourceProjName(proj_name)
	return "Resources_of_" .. proj_name
end

-- =============================================================================
-- Function to require copying a file or directory
-- =============================================================================

-- for file copying (from src to build directory),
-- some paths start with @ -> in that case, the file is copied in the build directory directly (no matter what SRC relative path is) (useful for DLL)
--
--      src/toto/titi/file.txt => build/file.txt
--
-- some does not -> they are copied in the equivalent path
--
--      src/toto/titi/file.txt => build/toto/titi/file.txt
--
-- TO_COPY is an array of {dst_path, src_path}

-- add input (whether it is a string or a table) into TO_COPY
function DeclareToCopyFileHelper(proj, filenames, plat, conf)
	ForEachElement(filenames,
		function(filename)
			local src_path = ""
			if (string.sub(filename, 1, 1) == "@") then -- the file is to be copied directly in the same directory than the executable itself
				filename = string.sub(filename, 2)
				src_path = path.join(proj.root_path, filename)
				filename = path.getname(filename) -- remove the path before the filename
			else
				src_path = path.join(proj.root_path, filename)
			end
			table.insert(proj.tocopy[plat][conf], {filename, src_path})
		end
	)
end

-- add a file/directory in the TO_COPY list
function DeclareToCopyFile(filename, proj)
	proj = proj or FindProject()
	local tmp = GetPlatConfArray(filename)
	AllTargets(
		function(plat, conf)
			DeclareToCopyFileHelper(proj, tmp[plat][conf], plat, conf)
		end
	)
end

-- =============================================================================
-- Function to initialize project for
-- =============================================================================

-- note on staticruntime -> dll or static
--         runtime       -> debug or rekease
-- /MT static multithread
-- /MTd static debug multithread
-- /MD  DLL
-- /MDd DLL debug

-- LinkTimeOptimization -> see /LTCG

	--staticruntime "on"
	--runtime "debug"
	--flags {"LinkTimeOptimization"}

function DebugConf(plat)
	filter {"configurations:" .. DEBUG, "platforms:" .. plat}
	defines {DEBUG}
	defines {"_DEBUG"}
	symbols "On"
	flags {"MultiProcessorCompile"}
end

-- =============================================================================
-- Function to initialize project for RELEASE
-- =============================================================================

	--staticruntime "on"

function ReleaseConf(plat)
	filter {"configurations:" .. RELEASE, "platforms:" .. plat}
	runtime "release"
	defines {"NDEBUG"}
	defines {RELEASE}
	defines {"_RELEASE"}
	optimize "On"
	symbols "Off"
	flags {"MultiProcessorCompile"}
end

-- =============================================================================
-- Function to find a project in MYPROJECTS
-- =============================================================================

function FindProject(name) -- by default, this returns the current project
	name = string.upper(name or PROJ_NAME)
	return MYPROJECTS[name]
end

-- =============================================================================
-- DeclareExternalLib
-- =============================================================================

-- add a prefix to a path (the path is a table[PLATFORM][CONFIG] format)
function PrefixPathArray(src, prefix)
	AllTargets(
		function(plat, conf)
			if (src[plat][conf]) then
				src[plat][conf] = path.join(prefix, src[plat][conf])
			end
		end
	)
	return src
end

function DeclareExternalLib(external_name, inc_path, lib_path, libname, tocopy)

	external_name = string.upper(external_name)

  Output("DeclareExternalLib [" .. external_name .. "]")
	if (MYPROJECTS[external_name]) then
		assert(false, "Project " .. external_name .. " already definied")
	end

	local result = {
		name = external_name,
		proj_type = TYPE_EXTERNAL_LIBRARY,
		root_path = EXTERNAL_PATH,
		includedirs = PrefixPathArray(GetPlatConfArray(inc_path), EXTERNAL_PATH),
		targetdir = PrefixPathArray(GetPlatConfArray(lib_path), EXTERNAL_PATH),
		libname = GetPlatConfArray(libname),
		additionnal_libs = GetPlatConfArray({}),
		dependencies = {},
		tocopy = GetPlatConfArray({})
	}

	MYPROJECTS[result.name] = result

	if (not IsNil(tocopy)) then
		Output("to copy " .. external_name)
		DeclareToCopyFile(tocopy, result)
	end

	-- check for library file existence
	AllTargets(
		function(plat, conf)
			ForEachElement(result.libname[plat][conf],
				function(libname)
					ForEachElement(result.targetdir[plat][conf],
						function(dir)
							local fullpath = dir .. "/" .. libname
							if not os.isfile(fullpath) then
								assert(false, "library does not exit: " .. fullpath)
							end
						end
					)
				end
			)
		end
	)
	return result

end

-- =============================================================================
-- GenDoxygen : doxygen generation
-- =============================================================================

function GenDoxygen()
	FindProject().gendoxygen = true
end

-- =============================================================================
-- GenZIP : Zip generation
-- =============================================================================

function GenZIP()
	FindProject().genzip = true
end

-- =============================================================================
-- OnConfig
-- =============================================================================

function OnConfig(in_kind, proj_type, plat, conf, proj)

	-- where the result EXE/LIB is been saved
	local targ = path.join(PROJECT_BUILD_PATH, plat, conf)
	targetdir(targ)
	proj.targetdir[plat][conf] = targ

	-- by default, the editor start the exe in the source path. We prefere to start where it has been build
	debugdir("$(TargetDir)")

	-- where the includes are for the project
	local inc = path.join(PROJECT_SRC_PATH, "include")
	includedirs(inc)
	proj.includedirs[plat][conf] = inc

	-- some definition for FILE REDIRECTION
	if (proj_type == TYPE_EXECUTABLE) then
		defines('CHAOS_PROJECT_PATH="' .. Base64Encode(PROJECT_PATH) .. '"')
		defines('CHAOS_PROJECT_SRC_PATH="' .. Base64Encode(PROJECT_SRC_PATH) .. '"')
		defines('CHAOS_PROJECT_BUILD_PATH="' .. Base64Encode(targ) .. '"')
		prebuildcommands('{ECHO} CHAOS_PROJECT_PATH       = "' .. PROJECT_PATH .. '"')
		prebuildcommands('{ECHO} CHAOS_PROJECT_SRC_PATH   = "' .. PROJECT_SRC_PATH .. '"')
		prebuildcommands('{ECHO} CHAOS_PROJECT_BUILD_PATH = "' .. targ .. '"')
	end

	characterset("ASCII")
end

-- =============================================================================
-- CppProject : entry point for all C++ Applications
-- =============================================================================

function CppProject(in_kind, proj_type)

	local name = string.upper(PROJ_NAME)

  Output("CppProject [" .. name .. "]")
	if (MYPROJECTS[name]) then
		assert(false, "Project " .. name .. " already definied")
	end

	-- the name of the group
	local group_name = path.join(CURRENT_GROUP, PROJ_NAME)
	if (CURRENT_GROUP ~= nil) then
		group(group_name)
	end

	-- create a project for the resources
	local proj_location = path.join(SOLUTION_PATH, "resources")
	local res_path = path.join(PROJECT_SRC_PATH, "resources")

	local resource_proj_name = GetDependantResourceProjName(PROJ_NAME)
	project(resource_proj_name)
	kind("Makefile")
	location(proj_location)
	files {path.join(res_path, "**")}

	-- create the project itself
	project(PROJ_NAME)
	kind(in_kind)
	location(proj_location)

	local proj_location = path.join(SOLUTION_PATH, PROJECT_PATH)
	local inc_path = path.join(PROJECT_SRC_PATH, "include")
	local src_path = path.join(PROJECT_SRC_PATH, "src")
	local res_path = path.join(PROJECT_SRC_PATH, "resources")

	local result = {
		name = name,
		proj_type = proj_type,
		path = PROJECT_PATH,
		root_path = PROJECT_SRC_PATH,
		build_path = PROJECT_BUILD_PATH,
		lua_project = project(),
		targetdir = GetPlatConfArray({}),
		includedirs = GetPlatConfArray({}),
		tocopy = GetPlatConfArray({}),
		gendoxygen = false,
		genzip = false,
		group_name = group_name,
		proj_location = proj_location,
		additionnal_libs = GetPlatConfArray({}),
		inc_path = GetPlatConfArray(inc_path),
		src_path = GetPlatConfArray(src_path),
		res_path = GetPlatConfArray(res_path),
		dependencies = {}
	}

	MYPROJECTS[result.name] = result

	language "C++"
	cppdialect "C++20"
		--staticruntime "on"

	-- change entry point for windows (avoid WinMain to main)
	if (os.target() == "windows") then
		if (proj_type == TYPE_EXECUTABLE) then
			entrypoint "mainCRTStartup"
		end
	end

	-- some files including *.cpp, *.c, *.hpp, *.h
	local src_h = path.join(PROJECT_SRC_PATH, "**.h")
	local src_hpp = path.join(PROJECT_SRC_PATH, "**.hpp")
	local src_c = path.join(PROJECT_SRC_PATH, "**.c")
	local src_cpp = path.join(PROJECT_SRC_PATH, "**.cpp")
	local src_ixx = path.join(PROJECT_SRC_PATH, "**.ixx")
	files {src_h, src_hpp, src_c, src_cpp, src_ixx}

	-- handle C++ modules
	filter {"files:**.ixx" }
		buildaction "ClCompile"
		compileas "Module"
	filter { }

	-- release/debug settings
	AllTargets(
		function(plat, conf)
			if (conf == DEBUG) then
				DebugConf(plat)
			else
				ReleaseConf(plat)
			end
			OnConfig(in_kind, proj_type, plat, conf, result)
		end
	)

	return result
end

-- =============================================================================
-- WindowedApp : entry point WindowedApp
-- =============================================================================

function WindowedApp()
	local result = CppProject("WindowedApp", TYPE_EXECUTABLE)
	DisplayEnvironment()
	GenZIP()
	return result
end

-- =============================================================================
-- Library
-- =============================================================================

function Library(kind_library)
	local result = CppProject(kind_library, TYPE_LIBRARY)
	result.libname = GetPlatConfArray(result.name)
	DisplayEnvironment()
	DeclareToCopyFile("resources")
	GenDoxygen()
	return result
end

function StaticLibrary()
	Library("StaticLib")
end

function SharedLibrary()
	Library("SharedLib")
end

-- =============================================================================
-- ResourceLib : entry point resources only libraries
-- =============================================================================

function ResourceLib()

	local name = string.upper(PROJ_NAME)

  Output("ResourceLib [" .. name .. "]")
	if (MYPROJECTS[name]) then
		assert(false, "Project " .. name .. " already definied")
	end

	-- the name of the group
	local group_name = path.join(CURRENT_GROUP, PROJ_NAME)
	if (CURRENT_GROUP ~= nil) then
		group(group_name)
	end

	-- create the project it self
	local proj_location = path.join(SOLUTION_PATH, PROJECT_PATH)
	local res_path = path.join(PROJECT_SRC_PATH, "resources")

	project(PROJ_NAME)
	location(proj_location)
	files {path.join(res_path, "**")}

	local result = {

		name = name,
		proj_type = TYPE_RESOURCES,
		path = PROJECT_PATH,
		root_path = PROJECT_SRC_PATH,
		build_path = PROJECT_BUILD_PATH,
		lua_project = project(),
		tocopy = GetPlatConfArray({}),
		gendoxygen = false,
		genzip = false,
		group_name = group_name,
		proj_location = proj_location,
		dependencies = {},
		res_path = GetPlatConfArray(res_path)
	}

	MYPROJECTS[result.name] = result

	kind("None")

	DisplayEnvironment()
	DeclareToCopyFile("resources")

	return result

end

-- =============================================================================
-- DependOnStandardLib : declare some additionnal dependency (does not depend on project)
-- =============================================================================

function DependOnStandardLib(libname)
	if os.target() ~= "windows" then
		return
	end

	ForEachElement(libname,
		function(lib)
			local proj = FindProject()
			AllTargets(
				function(plat, conf)
					table.insert(proj.additionnal_libs[plat][conf], libname)
				end
			)
		end
	)
end

-- =============================================================================
-- DependOnLib : declare a dependency
-- =============================================================================

function DependOnLib(libnames)
	ForEachElement(libnames,
		function(libname)
			table.insert(FindProject().dependencies, string.upper(libname))
		end
	)
end

-- =============================================================================
-- Processing sub premakes
-- =============================================================================

-- returns an ENV array that contains all required members from _G
function StoreEnvironment(names, result)
	result = result or {}
	for k, v in pairs(names) do
		result[v] = _G[v]
	end
	return result
end

-- take an ENV and write back all members to _G
function RestoreEnvironment(env)
	for k, v in pairs(env) do
		_G[k] = v
	end
end

-- process a subpremake file (store then restore environment)
function ProcessSubPremake(proj_name)

	Output("SUBDIRECTORY [" .. proj_name .. "]")

	local env = StoreEnvironment({"INDENT", "CURRENT_GROUP", "PROJECT_PATH", "PROJ_NAME", "PROJECT_SRC_PATH", "BUILD_PATH", "PROJECT_BUILD_PATH"})

	INDENT = INDENT + 1
	PROJ_NAME = string.upper(path.getbasename(proj_name))
	PROJECT_PATH = path.join(PROJECT_PATH, path.getbasename(proj_name))
	PROJECT_SRC_PATH = path.join(ROOT_PATH, PROJECT_PATH)
	PROJECT_BUILD_PATH = path.join(BUILD_PATH, PROJECT_PATH)

	include(path.join(PROJECT_SRC_PATH, "subpremake5.lua"))
	RestoreEnvironment(env)

end