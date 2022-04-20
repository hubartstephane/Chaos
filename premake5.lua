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
-- global variables
-- =============================================================================

TYPE_EXECUTABLE = 0
TYPE_EXECUTABLE = 0
TYPE_LIBRARY = 1
TYPE_EXTERNAL_LIBRARY = 2
TYPE_RESOURCES = 3

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
PROJ_NAME = ""
PROJECT_PATH = ""
PROJECT_SRC_PATH = ""
PROJECT_BUILD_PATH = ""

CURRENT_GROUP = nil

MYPROJECTS = {}

DISPLAY_ROOT_ENVIRONMENT = true
DISPLAY_ENVIRONMENT = false
DISPLAY_DEPENDENCIES = true

DEBUG = "DEBUG"
RELEASE = "RELEASE"
x32 = "x32"
x64 = "x64"
win32 = "win32"
win64 = "win64"

PLATFORMS = {x64}
CONFIGS = {DEBUG, RELEASE}

INDENT = 1

require 'premake5_internal'
require 'external_premake5'

DisplayRootEnvironment()
DisplayEnvironment()

-- =============================================================================
-- Entry point
-- =============================================================================

solution "Chaos"

	platforms {table.unpack(PLATFORMS)}
	configurations {table.unpack(CONFIGS)}

	location(SOLUTION_PATH) -- where the visual studio project file is been created

	if os.target() == "windows" then
		defines {"WINDOWS"}
	end

	if os.target() == "linux" then
		defines {"LINUX"}
	end

	for k, v in pairs({"libraries", "executables", "shared_resources"}) do
		CURRENT_GROUP = v
		ProcessSubPremake(v)
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
	-- resolve with TYPE_LIBRARY or TYPE_EXTERNAL_LIBRARY
	if (other_proj.proj_type == TYPE_LIBRARY or other_proj.proj_type == TYPE_EXTERNAL_LIBRARY) then -- only resolve dependencies with libraries

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
	if (proj.proj_type == TYPE_EXECUTABLE or proj.proj_type == TYPE_LIBRARY) then -- only compilable projects need some special link & include
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
	if (proj.proj_type == TYPE_EXECUTABLE) then
		project(proj.name)
		AllTargets(
			function(plat, conf)
				filter {"configurations:" .. conf, "platforms:" .. plat}
				local resource_path = ""
				for k, other_proj in pairs(proj.full_dependencies) do
					if (other_proj and (other_proj.proj_type == TYPE_RESOURCES or other_proj.proj_type == TYPE_LIBRARY)) then
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
		kind("Makefile")

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
	if (proj.genzip and proj.proj_type == TYPE_EXECUTABLE) then
		local zip_proj_name = GetDependantZipProjName(proj.name)

		group(proj.group_name) -- same group than the library
		project(zip_proj_name)
		kind("Makefile")

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
	project(GetDependantResourceProjName(p.name)) -- files are copied only when the "resource project" is being build

	local all_files = src_proj.tocopy[plat][conf]
	if (all_files) then
		filter {"configurations:" .. conf, "platforms:" .. plat}
		for v, data in pairs(all_files) do
			local filename = data[1]
			local full_filename = data[2]
			local dst_name = path.join(dst_proj.targetdir[plat][conf], filename)
			-- commands
			local build_command_str = QuotationMarks(COPY_SCRIPT, full_filename, dst_name)
			buildcommands(build_command_str)
			local clean_command_str = QuotationMarks(CLEAN_SCRIPT, dst_name)
			cleancommands(clean_command_str)
		end
	end
	project(p.name) -- restore the project
end

for k, proj in pairs(MYPROJECTS) do
	if (proj.proj_type == TYPE_EXECUTABLE) then
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

