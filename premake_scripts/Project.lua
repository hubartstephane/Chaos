require "Object"

--------------------------------------------------------------------
-- The types of projects (the values are valid strings for premake5.kind(...) function)
--------------------------------------------------------------------
ProjectType = {
	WINDOW_EXE       = "WindowedApp",
	CONSOLE_EXE      = "ConsoleApp",
	STATIC_LIBRARY   = "StaticLib",
	SHARED_LIBRARY   = "SharedLib",
	EXTERNAL_LIBRARY = "External Library", -- except for this
	RESOURCES        = "Makefile"
}

--------------------------------------------------------------------
-- Class declaration
--------------------------------------------------------------------
Project = Object:new({
	project_name       = "",
	project_type       = "",
	project_path       = "",
	project_src_path   = "",
	project_build_path = "",
	current_group      = "",
	gen_zip            = false,
	gen_doxygen        = false
})

--------------------------------------------------------------------
-- type of the project
--------------------------------------------------------------------

function Project:IsExecutable()
	return (self.project_type == ProjectType.WINDOW_EXE or self.project_type == ProjectType.CONSOLE_EXE)
end

function Project:IsStaticLibrary()
	return (self.project_type == ProjectType.STATIC_LIBRARY)
end

function Project:IsSharedLibrary()
	return (self.project_type == ProjectType.SHARED_LIBRARY)
end

function Project:IsLibrary()
	return (self:IsStaticLibrary() or self:IsSharedLibrary())
end

function Project:IsResources()
	return (self.project_type == ProjectType.RESOURCES)
end

--------------------------------------------------------------------
-- Display project information
--------------------------------------------------------------------

function Project:DisplayInformation()
	Log:Output("=======================================================================")
	Log:Output("== " .. self.project_name)
	Log:Output("=======================================================================")

	-- depend generic information
	Log:Output("PROJECT_NAME       : " .. self.project_name)
	Log:Output("PROJECT_TYPE       : " .. self.project_type)
	Log:Output("PROJECT_PATH       : " .. self.project_path)
	Log:Output("PROJECT_SRC_PATH   : " .. self.project_src_path)
	Log:Output("PROJECT_BUILD_PATH : " .. self.project_build_path)
	Log:Output("CURRENT_GROUP      : " .. self.current_group)
	Log:Output("GEN_DOXYGEN        : " .. tostring(self.gen_doxygen))
	Log:Output("GEN_ZIP            : " .. tostring(self.gen_zip))
	if (self.pch_header) then
		Log:Output("PCH_HEADER         : " .. self.pch_header)
	end
	if (self.pch_source) then
		Log:Output("PCH_SOURCE         : " .. self.pch_source)
	end

	-- some configurations
	if (self.targetdir or self.includedirs or self.lib_name or self.tocopy) then
		Log:Output("")
		Utility:AllTargets(
			function(plat, conf)
				Log:Output("[" .. plat .. "][" .. conf .. "]")
				Utility:DisplayPlatConfArray(self.targetdir,        "targetdir  ", plat, conf)
				Utility:DisplayPlatConfArray(self.includedirs,      "includedirs", plat, conf)
				Utility:DisplayPlatConfArray(self.lib_name,         "lib_name    ", plat, conf)
				Utility:DisplayPlatConfArray(self.additionnal_libs, "extra libs ", plat, conf)
				Utility:DisplayPlatConfArray(self.tocopy,           "tocopy     ", plat, conf)
				Log:Output("")
			end
		)
	end

	-- display dependencies
	for _, proj in ipairs(self.dependencies) do
		Log:Output("depend on: " .. proj.project_name)
	end

	Log:Output("")
end

--------------------------------------------------------------------
-- add a file/directory in the TO_COPY list
--------------------------------------------------------------------

function Project:AddFileToCopyExt(filenames, plat, conf)
	Utility:ForEachElement(filenames,
		function(filename)
			table.insert(self.tocopy[plat][conf], filename)
		end
	)
end

function Project:AddFileToCopy(filename)
	local tmp = Utility:GetPlatConfArray(filename)
	Utility:AllTargets(
		function(plat, conf)
			self:AddFileToCopyExt(tmp[plat][conf], plat, conf)
		end
	)
end

--------------------------------------------------------------------
-- require documentation generation
--------------------------------------------------------------------
function Project:GenDoxygen()
	self.gen_doxygen = true
end

--------------------------------------------------------------------
-- require ZIP generation
--------------------------------------------------------------------
function Project:GenZIP()
	self.gen_zip = true
end

--------------------------------------------------------------------
-- declare a dependency
--------------------------------------------------------------------
function Project:DependOnLib(lib_names)
	Utility:ForEachElement(lib_names,
		function(lib_name)
			table.insert(self.dependencies, string.upper(lib_name))
		end
	)
end

--------------------------------------------------------------------
-- declare some additionnal dependencies (does not depend on any project)
--------------------------------------------------------------------
function Project:DependOnStandardLib(lib_name)
	if os.target() ~= "windows" then
		return
	end
	Utility:ForEachElement(lib_name,
		function(lib)
			Utility:AllTargets(
				function(plat, conf)
					table.insert(self.additionnal_libs[plat][conf], lib_name)
				end
			)
		end
	)
end

--------------------------------------------------------------------
-- Iterate over project and its dependencies
--------------------------------------------------------------------

function Project:ForProjectAndDependencies(func)
	func(self, true)
	for _, depend_project in ipairs(self.dependencies) do
		func(depend_project, false)
	end
end

--------------------------------------------------------------------
-- gets the name of the premake project for documentation
--------------------------------------------------------------------
function Project:GetDocumentationProjectName()
	return self.project_name .. " (Documentation)"
end

--------------------------------------------------------------------
-- gets the name of the premake project for ZIP
--------------------------------------------------------------------
function Project:GetZipProjectName()
	return self.project_name .. " (ZIP)"
end

--------------------------------------------------------------------
-- gets the name of the premake project for resources
--------------------------------------------------------------------
function Project:GetResourceProjectName()
	return self.project_name .. " (Resources)" 
end

--------------------------------------------------------------------
-- function to initialize project for DEBUG
--------------------------------------------------------------------

-- note on staticruntime -> dll or static
--               runtime -> debug or release
-- /MT static multithread
-- /MTd static debug multithread
-- /MD	DLL
-- /MDd DLL debug
--
-- LinkTimeOptimization -> see /LTCG
-- staticruntime "on"
-- runtime "debug"
-- flags {"LinkTimeOptimization"}

function Project:DebugConf(plat)
	filter {"configurations:" .. DEBUG, "platforms:" .. plat}
	defines {DEBUG}
	defines {"_DEBUG"}
	staticruntime "off"
	symbols "On"
	flags {"MultiProcessorCompile"}
end


--------------------------------------------------------------------
-- function to initialize project for RELEASE
--------------------------------------------------------------------
function Project:ReleaseConf(plat)
	filter {"configurations:" .. RELEASE, "platforms:" .. plat}
	runtime "release"
	defines {"NDEBUG"}
	defines {RELEASE}
	defines {"_RELEASE"}
	staticruntime "off"
	optimize "On"
	symbols "Off"
	flags {"MultiProcessorCompile"}
	flags {"LinkTimeOptimization"}
end

--------------------------------------------------------------------
-- generic initializations
--------------------------------------------------------------------

function Project:OnConfig(plat, conf)

	targetdir(self.targetdir[plat][conf])
	includedirs(self.includedirs[plat][conf])
	-- some definition for FILE REDIRECTION
	if (self:IsExecutable()) then

		local resource_path = ""
		self:ForProjectAndDependencies(
			function(p, is_self)
				if (p:IsResources() or p:IsExecutable() or p:IsLibrary()) then
					if (resource_path == "") then
						resource_path = p.project_src_path
					else
						resource_path = resource_path .. ";" .. p.project_src_path
					end
				end
			end
		)

		defines('DEATH_PROJECT_BUILD_PATH="' .. Utility:Base64Encode(self.targetdir[plat][conf]) .. '"')
		defines('DEATH_PROJECT_DIRECT_RESOURCE_PATH="' .. Utility:Base64Encode(resource_path) .. '"')

		prebuildcommands('{ECHO} DEATH_PROJECT_BUILD_PATH		    = "' .. self.targetdir[plat][conf] .. '"')
		prebuildcommands('{ECHO} DEATH_PROJECT_DIRECT_RESOURCE_PATH = "' .. resource_path .. '"')

	end

end

--------------------------------------------------------------------
-- Create a sub-premake project for ZIP
--------------------------------------------------------------------

function Project:AddResourceProjectToSolution()

	local resource_proj_name = self:GetResourceProjectName()
	project(resource_proj_name)

	local proj_location = path.join(SOLUTION_PATH, self.project_path)
	location(proj_location)

	kind("Makefile")

	local res_path = path.join(self.project_src_path, "resources")
	files {path.join(res_path, "**")}

	Utility:AllTargets(
		function(plat, conf)
			self:ForProjectAndDependencies(
				function(p, is_self)
					local all_files = p.tocopy[plat][conf] -- copy from linked project
					if (all_files) then
						for _, filename in pairs(all_files) do

							local src_path = path.join(p.project_src_path, filename)

							local is_dll = (string.upper(path.getextension(filename)) == ".DLL")

							-- copy for standard files
							if (not is_dll) then

								local dst_path = path.join(self.project_build_path, plat, conf, path.getname(filename))
								project(resource_proj_name) -- for resource project
								filter {"configurations:" .. conf, "platforms:" .. plat}
								local build_command_str = Utility:QuotationMarks(COPY_SCRIPT, src_path, dst_path)
								buildcommands(build_command_str)
								local clean_command_str = Utility:QuotationMarks(CLEAN_SCRIPT, dst_path)
								cleancommands(clean_command_str)
							-- create a dependancy for dll
							else
								project(self.project_name) -- for main project
								filter {"configurations:" .. conf, "platforms:" .. plat}
								files {src_path}
							end
						end
					end
				end
			)
		end
	)

	project(self.project_name) -- restore base project
	filter {}

end

--------------------------------------------------------------------
-- Create a sub-premake project for ZIP
--------------------------------------------------------------------
function Project:AddZipProjectToSolution()

	if (self:IsExecutable() and self.gen_zip) then

		local zip_project_name = self:GetZipProjectName()
		project(zip_project_name)

		local proj_location = path.join(SOLUTION_PATH, self.project_path)
		location(proj_location)

		kind("Makefile")

		Utility:AllTargets(
			function(plat, conf)
				filter {"configurations:" .. conf, "platforms:" .. plat}

				local zip_path = path.join(ZIP_PATH, self.project_name) .. "_" .. conf .. "_" .. plat .. ".zip"

				local zip_command_str = Utility:QuotationMarks(ZIP_SCRIPT, self.targetdir[plat][conf], zip_path)
				buildcommands(zip_command_str)
				rebuildcommands(zip_command_str)

				local clean_command_str = Utility:QuotationMarks(CLEAN_SCRIPT, zip_path)
				cleancommands(clean_command_str)

				links(self.project_name)
				links(self:GetResourceProjectName())
			end
		)

	end

	project(self.project_name) -- restore base project
	filter {}

end

--------------------------------------------------------------------
-- Create a sub-premake project for DOC
--------------------------------------------------------------------
function Project:AddDocProjectToSolution()

	if (self:IsLibrary() and self.gen_doxygen) then

		local zip_project_name = self:GetDocumentationProjectName()
		project(zip_project_name)

		local proj_location = path.join(SOLUTION_PATH, self.project_path)
		location(proj_location)

		kind("Makefile")

		local build_command_str = Utility:QuotationMarks(DOXYGEN_SCRIPT, self.project_src_path, self.project_build_path, self.project_name)
		local doc_path = path.join(self.project_build_path, "html")
		local clean_command_str = Utility:QuotationMarks(CLEAN_SCRIPT, doc_path)

		Utility:AllTargets(
			function(plat, conf)
				filter {"configurations:" .. conf, "platforms:" .. plat}
				buildcommands(build_command_str)
				rebuildcommands(build_command_str)
				cleancommands(clean_command_str)
			end
		)
		links(self.project_name)

	end

	project(self.project_name) -- restore base project
	filter {}

end

--------------------------------------------------------------------
-- Add the project into premake solution
--------------------------------------------------------------------

function Project:AddProjectToSolution()

	-- ignore external project
	if (self.project_type == ProjectType.EXTERNAL_LIBRARY) then
		return
	end

	group(path.join(self.current_group, self.project_name))	-- valid for main project and dependant ones (resources, zip, doc)

	-- declare project
	project(self.project_name)

	local proj_location = path.join(SOLUTION_PATH, self.project_path)
	location(proj_location)
	
	-- put in solution src and headers into simple virtual directories
	-- for '..' not really sure of what is happening
	-- this is probably because premake5.lua is now in a sub directory and not directly a the root dir of the project
	vpaths { ["Headers"] = {"../**.h", "../**.hpp"} }
	vpaths { ["Sources"] = {"../**.c", "../**.cpp"} }	

	-- kind
	kind(self.project_type)
	allmodulespublic "on" -- required for DLL+modules (requires at least premake 5.0.0-beta2)

	if (self:IsSharedLibrary()) then
		defines('DEATH_BUILDING_SHARED_LIBRARY') -- indicates whether we are building a SHARED_LIBRARY (it does not indicate how a given project has been built)
	end
	if (self:IsStaticLibrary()) then
		defines('DEATH_BUILDING_STATIC_LIBRARY') -- indicates whether we are building a STATIC_LIBRARY (it does not indicate how a given project has been built)
	end
	if (self:IsExecutable()) then
		defines('DEATH_BUILDING_EXECUTABLE') -- indicates whether we are building an EXECUTABLE (it does not indicate how a given project has been built)
	end

	defines('DEATH_IS_BUILDING_' .. self.project_name) -- the name of the the project beeing built

	-- language/dialect
	language "C++"

	if (_ACTION == "codelite") then
		cppdialect "C++2a"
	else
		cppdialect "C++20"
	end

	-- entry point (avoid WinMain to main)
	if (os.target() == "windows") then
		if (self.project_type == ProjectType.WINDOW_EXE) then
			entrypoint "mainCRTStartup" -- this is important to avoid the default console to be opened
		end
	end

	-- config file
	if (self.generated_config_file) then
		self:HandleGeneratedConfigFile()
	end

	-- files to handle: *.cpp, *.c, *.hpp, *.h, *.ixx
	local src_h = path.join(self.project_src_path, "**.h")
	local src_hpp = path.join(self.project_src_path, "**.hpp")
	local src_c = path.join(self.project_src_path, "**.c")
	local src_cpp = path.join(self.project_src_path, "**.cpp")
	local src_ixx = path.join(self.project_src_path, "**.ixx")
	files {src_h, src_hpp, src_c, src_cpp, src_ixx}

	-- handle C++ modules
	filter {"files:**.ixx" }
		buildaction "ClCompile"
		compileas "Module"
	filter {}

	-- handle "natvis" files for visual studio
	filter "action:vs*"
		files { path.join(ROOT_PATH, "natvis", "glm.natvis") }
		files { path.join(ROOT_PATH, "natvis", "nlohmann_json.natvis") }
	filter {}

	-- release/debug settings
	Utility:AllTargets(
		function(plat, conf)

			filter {"configurations:" .. conf, "platforms:" .. plat}

			if (conf == DEBUG) then
				self:DebugConf(plat)
			else
				self:ReleaseConf(plat)
			end
			self:OnConfig(plat, conf)

			self:ForProjectAndDependencies(
				function(p, is_self)
					-- include path
					Utility:ForEachElement(p.includedirs[plat][conf],
						function(elem)
							includedirs(elem)
						end
					)
					-- link
					if (self:IsExecutable() or self:IsSharedLibrary()) then -- only executable/shared library should link to other libraries
						Utility:ForEachElement(p.additionnal_libs[plat][conf],
							function(elem)
								links(elem)
							end
						)
						if (not self:IsSharedLibrary() or not is_self) then -- a shared directory does not link to itself
							Utility:ForEachElement(p.lib_name[plat][conf],
								function(elem)
									links(elem)
								end

							)
						end
					end

					-- libraries/exe may face a #pragma comment(lib, ...) directive to require a link
					-- the path is needed in that case
					Utility:ForEachElement(p.targetdir[plat][conf],
						function(elem)
							libdirs(elem)
						end
					)
					
					--linkoptions
					Utility:ForEachElement(p.linkoptions[plat][conf],
						function(elem)
							linkoptions(elem)
						end
					)					
				end
			)
		end
	)
	filter {}

	-- by default, the editor start the exe in the source path. We prefere to start where it has been build
	debugdir("$(TargetDir)")

	-- characterset
	characterset("ASCII")

	-- special filter for copying dll
	filter 'files:**.dll'
		buildmessage 'DLL HANDLING %{file.basename}.dll'
		build_command_str = Utility:QuotationMarks(COPY_SCRIPT, '%{file.abspath}', '%{cfg.targetdir}/%{file.basename}.dll')
		buildcommands(build_command_str)
		buildoutputs '%{cfg.targetdir}/%{file.basename}.dll'

	-- depend projects
	self:AddResourceProjectToSolution()
	self:AddZipProjectToSolution()
	self:AddDocProjectToSolution()

	-- add precompiled headers
	self:HandlePrecompiledHeader()

end

function Project:HandleGeneratedConfigFile()

	local patterns = { PROJECT_NAME = self.project_name}

	if (self:IsExecutable()) then
		patterns["PROJECT_BUILD_TYPE"] = self.project_name .. "_IS_EXECUTABLE"
	elseif (self:IsStaticLibrary()) then
		patterns["PROJECT_BUILD_TYPE"] = self.project_name .. "_IS_STATIC_LIBRARY"
	elseif (self:IsSharedLibrary()) then
		patterns["PROJECT_BUILD_TYPE"] = self.project_name .. "_IS_SHARED_LIBRARY"
	end

	if (os.isfile(CONFIG_TEMPLATE_PATH)) then

		-- read the config template file, split into line, make replacements
		local content = io.readfile(CONFIG_TEMPLATE_PATH)
		for k, v in pairs(patterns) do
			local pattern = '%$' .. k .. '%$'
			content = content:gsub(pattern, v)
		end
		-- write into destination file
		local dst_file = path.join(self.project_src_path,self.generated_config_file)
		io.writefile(dst_file, content)
	end
end

function Project:HandlePrecompiledHeader()

	if (self.pch_header) then
		pchheader(self.pch_header)
	end

	if (self.pch_source) then
		pchsource(path.join(".." , self.project_path, self.pch_source))
	end
end

function Project:PrecompiledHeader(header, source)
	self.pch_header = header
	self.pch_source = source
end

function Project:GenerateConfigFile(file)
	self.generated_config_file = file
end
