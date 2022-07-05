-- =============================================================================
-- Some windows command reminders
-- =============================================================================

-- WTF : the os.execute(...) seems to switch / and \ on arguments for windows ...
--       causing the command to fail
--
-- MKLINK /H   new_file   src_file                  (link between files)
-- MKLINK /J   new_dir    src_dir                   (link between directories)
-- =============================================================================
-- Project description
--
-- in some commands we can use some jockers such as
--   %{sln.name}
--   %{prj.location}
--
-- they are listed below:
--  sln.name
--
--  prj.name
--  prj.language
--
--  cfg.architecture
--  cfg.buildcfg
--  cfg.buildtarget
--  cfg.kind
--  cfg.linktarget
--  cfg.longname
--  cfg.objdir
--  cfg.platform
--  cfg.shortname
--  cfg.system
--
--  file.abspath
--  file.basename
--  file.name
--  file.relpath
--       These values are available on build and link targets
--  target.abspath
--  target.basename
--  target.bundlename
--  target.bundlepath
--  target.directory
--  target.extension
--  target.name
--  target.prefix
--  target.relpath
--  target.suffix
--
-- And generic strings for portable commands:
--  {CHDIR}
--  {COPY}
--  {DELETE}
--  {ECHO}
--  {MKDIR}
--  {MOVE}
--  {RMDIR}
--  {TOUCH}

-- =============================================================================
-- Object
-- =============================================================================

Object = {}

function Object:new(data)

  data = data or {}
  setmetatable(data, self)
  self.__index = self
  return data
end

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
-- global variables
-- =============================================================================

ProjectType = {  
	EXECUTABLE = {},
	STATIC_LIBRARY = {},
	SHARED_LIBRARY = {},
	EXTERNAL_LIBRARY = {},
	RESOURCES = {}    
}

BUILD_TARGET = _ACTION
ROOT_PATH = path.getdirectory(_SCRIPT)
SOLUTION_PATH = path.join(ROOT_PATH, "solutions", BUILD_TARGET)
EXTERNAL_PATH = path.join(ROOT_PATH, "external")
BUILD_PATH = path.join(ROOT_PATH, "build", BUILD_TARGET)
BUILD_TOOLS_PATH = path.join(ROOT_PATH, "build_tools")
ZIP_PATH = path.join(ROOT_PATH, "zip")
COPY_SCRIPT = path.join(BUILD_TOOLS_PATH, "mycopy.py")
ZIP_SCRIPT = path.join(BUILD_TOOLS_PATH, "myzip.py")
DOXYGEN_SCRIPT = path.join(BUILD_TOOLS_PATH, "mydoxygen.py")
CLEAN_SCRIPT = path.join(BUILD_TOOLS_PATH, "myclean.py")





CURRENT_GROUP = nil

MYPROJECTS = {}

DISPLAY_ROOT_ENVIRONMENT = true
DISPLAY_ENVIRONMENT = false
DISPLAY_DEPENDENCIES = false

DEBUG = "DEBUG"
RELEASE = "RELEASE"
x32 = "x32"
x64 = "x64"
win32 = "win32"
win64 = "win64"

PLATFORMS = {x64} -- no more 32bit support
CONFIGS = {DEBUG, RELEASE}



SPECIAL_PROJECT="Makefile"


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

function Project:GetDocumentationProjName()
	return "Documentation_of_" .. self.proj_name
end

function Project:GetZipProjName()
	return "ZIP_of_" .. self.proj_name
end

function Project:GetResourceProjName()
	return "Resources_of_" .. self.proj_name
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
-- Build class
-- =============================================================================

Build = Object:new({
	INDENT = 1,
	PROJ_NAME = "",
	PROJECT_PATH = "",
	PROJECT_SRC_PATH = "",
	PROJECT_BUILD_PATH = ""
	PROJECTS = {}
})

-- =============================================================================
-- Function to find a project in MYPROJECTS
-- =============================================================================

function Build:FindProject(name) -- by default, this returns the current project
	name = string.upper(name or PROJ_NAME)
	return selft.PROJECTS[name]
end




-- =============================================================================
-- Project class
-- =============================================================================

Project = Object:new()



function StaticLibrary()
	
end

function SharedLibrary()
	
end

function SharedLibrary()
	
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
		proj_type = ProjectType.EXTERNAL_LIBRARY,
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

function Project:GenDoxygen()
	self.gendoxygen = true
end

-- =============================================================================
-- GenZIP : Zip generation
-- =============================================================================

function GenZIP()
	self.genzip = true
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
	if (proj_type == ProjectType.EXECUTABLE) then
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

.

-- =============================================================================
-- WindowedApp : entry point WindowedApp
-- =============================================================================

function WindowedApp()
	local result = CppProject("WindowedApp", ProjectType.EXECUTABLE)
	DisplayEnvironment()
	result:GenZIP()
	return result
end

-- =============================================================================
-- Library
-- =============================================================================

function Library(kind_library, type)
	local result = CppProject(kind_library, type)
	result.libname = GetPlatConfArray(result.name)
	DisplayEnvironment()
	DeclareToCopyFile("resources")
	if (type == ProjectType.SHARED_LIBRARY) then
		DeclareToCopyFile("@" .. path.join(result.targetdir[x64][DEBUG], result.name .. ".dll"))
	end
	GenDoxygen()
	return result
end




function StaticLibrary()
	return Library("StaticLib", ProjectType.STATIC_LIBRARY)
end

function SharedLibrary()
	local result = Library("SharedLib", ProjectType.SHARED_LIBRARY)
	filter {}
	defines('CHAOS_IS_BUILDING_DLL')
	allmodulespublic "on" -- required for DLL+modules (requires at least premake 5.0.0-beta2)
	return result
end














--[[--

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
		proj_type = ProjectType.RESOURCES,
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

	kind(SPECIAL_PROJECT)

	DisplayEnvironment()
	DeclareToCopyFile("resources")

	return result

end

--]]--











-- =============================================================================
-- DependOnStandardLib : declare some additionnal dependency (does not depend on project)
-- =============================================================================

function Project:DependOnStandardLib(libname)
	if os.target() ~= "windows" then
		return
	end

	ForEachElement(libname,
		function(lib)
			AllTargets(
				function(plat, conf)
					table.insert(self.additionnal_libs[plat][conf], libname)
				end
			)
		end
	)
end

-- =============================================================================
-- DependOnLib : declare a dependency
-- =============================================================================

function Project:DependOnLib(libnames)
	ForEachElement(libnames,
		function(libname)
			table.insert(self.dependencies, string.upper(libname))
		end
	)
end




































-- =============================================================================
-- Processing sub premakes
-- =============================================================================

-- returns an ENV array that contains all required members from self
function Build:StoreEnvironment(names, result)
	result = result or {}
	for k, v in pairs(names) do
		result[v] = self[v]
	end
	return result
end

-- take an ENV and write back all members to _G
function Build:RestoreEnvironment(env)
	for k, v in pairs(env) do
		self[k] = v
	end
end

-- process a subpremake file (store then restore environment)
function Build:ProcessSubPremake(dir_name)

	if (dir_name == nil) then
		
	else
		
	end


	Output("SUBDIRECTORY [" .. dir_name .. "]")

	local env = self:StoreEnvironment({"INDENT", "CURRENT_GROUP", "PROJECT_PATH", "PROJ_NAME", "PROJECT_SRC_PATH", "BUILD_PATH", "PROJECT_BUILD_PATH"})

	self.INDENT = INDENT + 1
	self.PROJ_NAME = string.upper(path.getbasename(dir_name))
	self.PROJECT_PATH = path.join(PROJECT_PATH, path.getbasename(dir_name))
	self.PROJECT_SRC_PATH = path.join(ROOT_PATH, PROJECT_PATH)
	self.PROJECT_BUILD_PATH = path.join(BUILD_PATH, PROJECT_PATH)

	include(path.join(PROJECT_SRC_PATH, "subpremake5.lua"))
	
	self:RestoreEnvironment(env)

end

-- =============================================================================
-- Entry point
-- =============================================================================

require 'external_premake5'
--require 'codeblocks'

--DisplayRootEnvironment()

--DisplayEnvironment()

-- =============================================================================
-- Solution
-- =============================================================================

solution "Chaos"

	platforms {table.unpack(PLATFORMS)}
	configurations {table.unpack(CONFIGS)}
	
	local arch = _OPTIONS['arch']
	if arch then
		Output("Architecture: " .. arch)
		architecture(arch)
	end

	location(SOLUTION_PATH) -- where the visual studio project file is been created

	if os.target() == "windows" then
		defines {"WINDOWS"}
	end

	if os.target() == "linux" then
		defines {"LINUX"}
	end

	--for k, v in pairs({"libraries", "executables", "shared_resources"}) do
	for k, v in pairs({"executables"}) do
		CURRENT_GROUP = v
		Build:ProcessSubPremake(v)
	end




















-- =============================================================================
-- Search all dependencies recursively: proj.full_dependencies
-- =============================================================================

function CollectDependencies(proj, collect_dependencies)
	if (IsTable(proj.dependencies)) then
		for k, v in pairs(proj.dependencies) do
			local other_proj = FindProject(v)
			if (not IsNil(other_proj)) then
				if (not SearchElementInTable(collect_dependencies, other_proj)) then
					table.insert(collect_dependencies, other_proj)
					CollectDependencies(other_proj, collect_dependencies)
				end
			else
				assert(false, "Dependency not found :" .. v .. " (for project " .. proj.name ..")")
			end
		end
	end
	return collect_dependencies
end

for k, proj in pairs(MYPROJECTS) do

	proj.full_dependencies = CollectDependencies(proj, {}) -- replace the array of string with an array of projects

	if (DISPLAY_DEPENDENCIES) then
		Output("=======================================================================")
		Output("[" .. proj.name .. "] Dependencies")
		Output("=======================================================================")
		for k, d in pairs(proj.full_dependencies) do
			Output("  " .. d.name)
		end
	end
end

-- =============================================================================
-- Link & include dependencies
-- =============================================================================

function ResolveDependency(proj, other_proj, plat, conf)
	-- resolve with ProjectType.STATIC_LIBRARY or ProjectType.SHARED_LIBRARY or ProjectType.EXTERNAL_LIBRARY
	if (other_proj.proj_type == ProjectType.STATIC_LIBRARY or other_proj.proj_type == ProjectType.SHARED_LIBRARY or other_proj.proj_type == ProjectType.EXTERNAL_LIBRARY) then -- only resolve dependencies with libraries

		filter {"configurations:" .. conf, "platforms:" .. plat}

		-- includes
		local inc_dir = other_proj.includedirs[plat][conf]
		ForEachElement(inc_dir,
			function(dir)
				includedirs(dir)
			end
		)
		-- libdirs
		local lib_dir = other_proj.targetdir[plat][conf] -- where is compiled the library
		ForEachElement(lib_dir,
			function(dir)
				libdirs(dir)
			end
		)
		-- links (additionnal libs)
		local lib_names = other_proj.libname[plat][conf]
		ForEachElement(lib_names,
			function(dir)
				links(dir)
			end
		)
		-- links (additionnal libs)
		local additionnal_libs = other_proj.additionnal_libs[plat][conf]
		ForEachElement(additionnal_libs,
			function(dir)
				links(dir)
			end
		)
	end
end

for k, proj in pairs(MYPROJECTS) do
	if (proj.proj_type == ProjectType.EXECUTABLE or proj.proj_type == ProjectType.STATIC_LIBRARY or proj.proj_type == ProjectType.SHARED_LIBRARY) then -- only compilable projects need some special link & include
		project(proj.name)
		AllTargets(
			function(plat, conf)
				-- required libs for this project
				local additionnal_libs = proj.additionnal_libs[plat][conf]
				ForEachElement(additionnal_libs,
					function(dir)
						links(dir)
					end
				)
				-- dependencies
				for k, other_proj in pairs(proj.full_dependencies) do
					if (other_proj) then
						ResolveDependency(proj, other_proj, plat, conf)
					end
				end
			end
		)
	end
end

-- =============================================================================
-- Direct file access
-- =============================================================================

for k, proj in pairs(MYPROJECTS) do
	if (proj.proj_type == ProjectType.EXECUTABLE) then
		project(proj.name)
		AllTargets(
			function(plat, conf)
				filter {"configurations:" .. conf, "platforms:" .. plat}
				local resource_path = ""
				for k, other_proj in pairs(proj.full_dependencies) do
					if (other_proj and (other_proj.proj_type == ProjectType.RESOURCES or other_proj.proj_type == ProjectType.STATIC_LIBRARY or other_proj.proj_type == ProjectType.SHARED_LIBRARY)) then
						if (resource_path ~= "") then
							resource_path = resource_path .. ";"
						end
						resource_path = resource_path .. other_proj.root_path
					end
				end
				if (resource_path ~= "") then
					defines('CHAOS_PROJECT_DIRECT_RESOURCE_PATH="' .. Base64Encode(resource_path) .. '"')
					prebuildcommands('{ECHO} CHAOS_PROJECT_DIRECT_RESOURCE_PATH = "' .. resource_path .. '"')
				end
			end
		)
	end
end

-- =============================================================================
-- Generate documentation
-- =============================================================================

for k, proj in pairs(MYPROJECTS) do
	if (proj.gendoxygen) then
		local resources_proj_name = GetDependantDocumentationProjName(proj.name)

		group(proj.group_name) -- same group than the library
		project(resources_proj_name)
		location(proj.proj_location)
		kind(SPECIAL_PROJECT)

		local build_command_str = QuotationMarks(DOXYGEN_SCRIPT, proj.root_path, proj.build_path, proj.name)
		local doc_path = path.join(proj.build_path, "html")
		local clean_command_str = QuotationMarks(CLEAN_SCRIPT, doc_path)

		AllTargets(
			function(plat, conf)
				filter {"configurations:" .. conf, "platforms:" .. plat}
				buildcommands(build_command_str)
				rebuildcommands(build_command_str)
				cleancommands(clean_command_str)
			end
		)
		links(proj.name)
	end
end

-- =============================================================================
-- Generate ZIP
-- =============================================================================

for k, proj in pairs(MYPROJECTS) do
	if (proj.genzip and proj.proj_type == ProjectType.EXECUTABLE) then
		local zip_proj_name = GetDependantZipProjName(proj.name)

		group(proj.group_name) -- same group than the library
		project(zip_proj_name)
		location(proj.proj_location)
		kind(SPECIAL_PROJECT)

		AllTargets(
			function(plat, conf)
				filter {"configurations:" .. conf, "platforms:" .. plat}

				local zip_path = path.join(ZIP_PATH, proj.name) .. "_" .. conf .. "_" .. plat .. ".zip"

				local zip_command_str = QuotationMarks(ZIP_SCRIPT, proj.targetdir[plat][conf], zip_path)
				buildcommands(zip_command_str)
				rebuildcommands(zip_command_str)

				local clean_command_str = QuotationMarks(CLEAN_SCRIPT, zip_path)
				cleancommands(clean_command_str)

				links(proj.name)
				links(GetDependantResourceProjName(proj.name))
			end
		)
	end
end

-- =============================================================================
-- Resources copying
-- =============================================================================

-- Dependencies resources are copied in reverse order so that last declared dependencies are overriden by first dependencies
-- The application PATH is build in reverse order so that direct file access is coherent with that

function CopyResourceFiles(dst_proj, src_proj, plat, conf) -- dst_proj is the project that wants resources

	local p = project()

	-- add copy commands
	local all_files = src_proj.tocopy[plat][conf]
	if (all_files) then
		for v, data in pairs(all_files) do
			local filename = data[1]
			local full_filename = data[2]
			local dst_name = path.join(dst_proj.targetdir[plat][conf], filename)

			local is_dll = (string.upper(path.getextension(filename)) == ".DLL")
			-- dll files are bound to normal project (and will be copyed into builddir)
			-- non dll files are handled by resource subproject
			if (not is_dll) then
				project(GetDependantResourceProjName(p.name))
				filter {"configurations:" .. conf, "platforms:" .. plat}
				local build_command_str = QuotationMarks(COPY_SCRIPT, full_filename, dst_name)
				buildcommands(build_command_str)
				local clean_command_str = QuotationMarks(CLEAN_SCRIPT, dst_name)
				cleancommands(clean_command_str)
			else
				project(p.name)
				filter {"configurations:" .. conf, "platforms:" .. plat}
				files {full_filename}
			end
		end
	end
	project(p.name) -- restore the project

end

for k, proj in pairs(MYPROJECTS) do
	if (proj.proj_type == ProjectType.EXECUTABLE) then
		project(proj.name)
		AllTargets(
			function(plat, conf)
				-- copy resources from direct dependencies (in reverse order)
				local count = #proj.full_dependencies
				for j = 1, count do
					local other_proj = proj.full_dependencies[count - j + 1]
					if (other_proj) then
						CopyResourceFiles(proj, other_proj, plat, conf)
					end
				end
				-- resources from executable itself must be visible
				CopyResourceFiles(proj, proj, plat, conf)
			end
		)
	end
end

