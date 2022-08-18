require "Object"

--------------------------------------------------------------------
-- The types of projects (the values are valid strings for premake5.kind(...) function)
--------------------------------------------------------------------
ProjectType = {
	WINDOW_EXE = "WindowedApp",
	CONSOLE_EXE = "ConsoleApp",
	STATIC_LIBRARY = "StaticLib",
	SHARED_LIBRARY = "SharedLib",
	EXTERNAL_LIBRARY = "External Library", -- except for this
	RESOURCES = "Makefile"
}

--------------------------------------------------------------------
-- Class declaration
--------------------------------------------------------------------
Project = Object:new({
	project_name = "",
	project_type = "",
	project_path = "",
	project_src_path = "",
	project_build_path = "",
	current_group = "",
	gen_zip = false,
	gen_doxygen = false
})

--------------------------------------------------------------------
-- type of the project
--------------------------------------------------------------------

function Project:IsExecutable()
	return (self.project_type == ProjectType.WINDOW_EXE or self.project_type == ProjectType.CONSOLE_EXE)
end

function Project:IsLibrary()
	return (self.project_type == ProjectType.STATIC_LIBRARY or self.project_type == ProjectType.SHARED_LIBRARY)
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

	-- some configurations
	if (self.targetdir or self.includedirs or self.libname or self.tocopy) then
		Log:Output("")
		Utility:AllTargets(
			function(plat, conf)
				Log:Output("[" .. plat .. "][" .. conf .. "]")
				Utility:DisplayPlatConfArray(self.targetdir,        "targetdir  ", plat, conf)
				Utility:DisplayPlatConfArray(self.includedirs,      "includedirs", plat, conf)
				Utility:DisplayPlatConfArray(self.libname,          "libname    ", plat, conf)
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

function Project:AddFileToCopyHelper(filenames, plat, conf)
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
			self:AddFileToCopyHelper(tmp[plat][conf], plat, conf)
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
function Project:DependOnLib(libnames)
	Utility:ForEachElement(libnames,
		function(libname)
			table.insert(self.dependencies, string.upper(libname))
		end
	)
end

--------------------------------------------------------------------
-- declare some additionnal dependencies (does not depend on any project)
--------------------------------------------------------------------
function Project:DependOnStandardLib(libname)
	if os.target() ~= "windows" then
		return
	end
	Utility:ForEachElement(libname,
		function(lib)
			Utility:AllTargets(
				function(plat, conf)
					table.insert(self.additionnal_libs[plat][conf], libname)
				end
			)
		end
	)
end

--------------------------------------------------------------------
-- Iterate over project and its dependencies
--------------------------------------------------------------------

function Project:ForProjectAndDependencies(func)
	func(self)
	for _, depend_project in ipairs(self.dependencies) do
		func(depend_project)
	end
end

--------------------------------------------------------------------
-- gets the name of the premake project for documentation
--------------------------------------------------------------------
function Project:GetDocumentationProjectName()
	return "Documentation_of_" .. self.project_name
end

--------------------------------------------------------------------
-- gets the name of the premake project for ZIP
--------------------------------------------------------------------
function Project:GetZipProjectName()
	return "ZIP_of_" .. self.project_name
end

--------------------------------------------------------------------
-- gets the name of the premake project for resources
--------------------------------------------------------------------
function Project:GetResourceProjectName()
	return "Resources_of_" .. self.project_name
end

--------------------------------------------------------------------
-- function to initialize project for DEBUG
--------------------------------------------------------------------

-- note on staticruntime -> dll or static
--				 runtime			 -> debug or rekease
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
			function(p)
				if (p:IsResources() or p:IsExecutable() or p:IsLibrary()) then
					if (resource_path == "") then
						resource_path = p.project_src_path
					else
						resource_path = resource_path .. ";" .. p.project_src_path
					end
				end
			end
		)

		defines('CHAOS_PROJECT_BUILD_PATH="' .. Utility:Base64Encode(self.targetdir[plat][conf]) .. '"')
		defines('CHAOS_PROJECT_DIRECT_RESOURCE_PATH="' .. Utility:Base64Encode(resource_path) .. '"')

		prebuildcommands('{ECHO} CHAOS_PROJECT_BUILD_PATH		    = "' .. self.targetdir[plat][conf] .. '"')
		prebuildcommands('{ECHO} CHAOS_PROJECT_DIRECT_RESOURCE_PATH = "' .. resource_path .. '"')

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
				function(p)
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
end

--------------------------------------------------------------------
-- Create a sub-premake project for DOC
--------------------------------------------------------------------
function Project:AddDocProjectToSolution()

	if (self:IsLibrary() and self.gen_doxygen) then

		print("AddDocProjectToSolution")

	end
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

	-- kind
	kind(self.project_type)
	if (self.project_type == ProjectType.SHARED_LIBRARY) then
		defines('CHAOS_IS_BUILDING_DLL')
		allmodulespublic "on" -- required for DLL+modules (requires at least premake 5.0.0-beta2)
	end

	-- language/dialect
	language "C++"

	if (_ACTION == "codelite") then
		cppdialect "C++2a"
	else
		cppdialect "C++20"
	end

	-- entry point (avoid WinMain to main)
	if (os.target() == "windows") then
		if (self:IsExecutable()) then
			--entrypoint "mainCRTStartup"
		end
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
				function(p)
					-- include path
					Utility:ForEachElement(p.includedirs[plat][conf],
						function(elem)
							includedirs(elem)
						end
					)
					-- link
					if (self:IsExecutable()) then -- only executable should link to other libraries
						Utility:ForEachElement(p.additionnal_libs[plat][conf],
							function(elem)
								links(elem)
							end
						)
						Utility:ForEachElement(p.libname[plat][conf],
							function(elem)
								links(elem)
							end
						)
						Utility:ForEachElement(p.targetdir[plat][conf],
							function(elem)
								libdirs(elem)
							end
						)
					end
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

end