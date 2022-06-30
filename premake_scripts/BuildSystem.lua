require "Object"
require "Utility"

--------------------------------------------------------------------
-- Class declaration
--------------------------------------------------------------------
BuildSystem = Object:new({
	current_group = "",
	project_name = "",
	project_path = "",
	project_src_path = "",
	project_build_path = "",
	projects = {}
})

--------------------------------------------------------------------
-- display the environment
--------------------------------------------------------------------
function BuildSystem:DisplayRootEnvironment()
	if (DISPLAY_ROOT_ENVIRONMENT) then
		Log:Output("=======================================================================")
		Log:Output("SCRIPT             : " .. _SCRIPT)
		Log:Output("PREMAKE5 VERSION   : " .. _PREMAKE_VERSION)
		Log:Output("BUILD_TARGET       : " .. BUILD_TARGET)
		Log:Output("ROOT_PATH          : " .. ROOT_PATH)
		Log:Output("ZIP_PATH           : " .. ZIP_PATH)
		Log:Output("EXTERNAL_PATH      : " .. EXTERNAL_PATH)
		Log:Output("BUILD_TOOLS_PATH   : " .. BUILD_TOOLS_PATH)
		Log:Output("COPY_SCRIPT        : " .. COPY_SCRIPT)
		Log:Output("ZIP_SCRIPT         : " .. ZIP_SCRIPT)
		Log:Output("DOXYGEN_SCRIPT     : " .. DOXYGEN_SCRIPT)
		Log:Output("CLEAN_SCRIPT       : " .. CLEAN_SCRIPT)
		Log:Output("=======================================================================")
	end
end

function BuildSystem:DisplayEnvironment()
	if (DISPLAY_ENVIRONMENT) then
		Log:Output("=======================================================================")
		Log:Output("PROJECT_NAME       : " .. self.project_name)
		Log:Output("PROJECT_PATH       : " .. self.project_path)
		Log:Output("PROJECT_SRC_PATH   : " .. self.project_src_path)
		Log:Output("PROJECT_BUILD_PATH : " .. self.project_build_path)
		Log:Output("CURRENT_GROUP      : " .. self.current_group)
		Log:Output("=======================================================================")
	end
end

--------------------------------------------------------------------
-- process a subpremake file (store then restore environment)
--------------------------------------------------------------------
function BuildSystem:ProcessSubPremake(dir_name, create_sub_group)

	Utility:ForEachElement(dir_name, 
		function(name)
			Log:Output("SUBDIRECTORY [" .. name .. "]")

			local env = self:StoreEnvironment({"current_group", "project_path", "project_name", "project_src_path", "project_build_path"})

			Log:IncrementIndent()
			
			if (create_sub_group) then
				self.current_group = path.join(self.current_group, name)
			end
			self.project_name = string.upper(path.getbasename(name))
			self.project_path = path.join(self.project_path, path.getbasename(name))
			self.project_src_path = path.join(ROOT_PATH, self.project_path)
			self.project_build_path = path.join(BUILD_PATH, self.project_path)
			include(path.join(self.project_src_path, "subpremake5.lua"))

			Log:DecrementIndent()

			self:RestoreEnvironment(env)
    end
  )
end

--------------------------------------------------------------------
-- ensure a project does not already exist then create it
--------------------------------------------------------------------
function BuildSystem:AddProject(name, data)
	
	self:DisplayEnvironment()
	
	local upper_name = string.upper(name)
	
	if (self.projects[upper_name]) then
		assert(false, "Project " .. upper_name .. " already definied")
	else
		local result = Project:new(data)
		result.name = name
		result.targetdir = Utility:GetPlatConfArray({})
		result.includedirs = Utility:GetPlatConfArray({})
		result.additionnal_libs = Utility:GetPlatConfArray({})
		result.dependencies = {}
		result.tocopy = Utility:GetPlatConfArray({})
		self.projects[upper_name] = result
		return result
	end
end

--------------------------------------------------------------------
-- declare an external library (not described by any premake project)
--------------------------------------------------------------------
function BuildSystem:DeclareExternalLib(external_name, inc_path, lib_path, libname, tocopy)

	Log:Output("DeclareExternalLib [" .. external_name .. "]")

	local result = self:AddProject(external_name, {
		proj_type = ProjectType.EXTERNAL_LIBRARY,
		root_path = EXTERNAL_PATH,
		includedirs = Utility:PrefixPathArray(Utility:GetPlatConfArray(inc_path), EXTERNAL_PATH),
		targetdir = Utility:PrefixPathArray(Utility:GetPlatConfArray(lib_path), EXTERNAL_PATH),
		libname = Utility:GetPlatConfArray(libname)
	})

	if (not Utility:IsNil(tocopy)) then
		result:AddFileToCopy(tocopy)
	end

	-- check for library file existence
	Utility:AllTargets(
		function(plat, conf)
			Utility:ForEachElement(result.libname[plat][conf],
				function(libname)
					Utility:ForEachElement(result.targetdir[plat][conf],
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

--------------------------------------------------------------------
-- function to initialize project for DEBUG
--------------------------------------------------------------------

-- note on staticruntime -> dll or static
--         runtime       -> debug or rekease
-- /MT static multithread
-- /MTd static debug multithread
-- /MD  DLL
-- /MDd DLL debug
--
-- LinkTimeOptimization -> see /LTCG
-- staticruntime "on"
-- runtime "debug"
-- flags {"LinkTimeOptimization"}

function BuildSystem:DebugConf(plat)
	filter {"configurations:" .. DEBUG, "platforms:" .. plat}
	defines {DEBUG}
	defines {"_DEBUG"}
	symbols "On"
	flags {"MultiProcessorCompile"}
end


--------------------------------------------------------------------
-- function to initialize project for RELEASE
--------------------------------------------------------------------

-- staticruntime "on"

function BuildSystem:ReleaseConf(plat)
	filter {"configurations:" .. RELEASE, "platforms:" .. plat}
	runtime "release"
	defines {"NDEBUG"}
	defines {RELEASE}
	defines {"_RELEASE"}
	optimize "On"
	symbols "Off"
	flags {"MultiProcessorCompile"}
end

--------------------------------------------------------------------
-- generic initializations
--------------------------------------------------------------------

function BuildSystem:OnConfig(in_kind, proj_type, plat, conf, proj)

	-- where the result EXE/LIB is been saved
	local targ = path.join(self.project_build_path, plat, conf)
	targetdir(targ)
	proj.targetdir[plat][conf] = targ

	-- by default, the editor start the exe in the source path. We prefere to start where it has been build
	debugdir("$(TargetDir)")

	-- where the includes are for the project
	local inc = path.join(self.project_src_path, "include")
	includedirs(inc)
	proj.includedirs[plat][conf] = inc

	-- some definition for FILE REDIRECTION
	if (proj_type == ProjectType.EXECUTABLE) then
		defines('CHAOS_PROJECT_PATH="' .. Utility:Base64Encode(self.project_path) .. '"')
		defines('CHAOS_PROJECT_SRC_PATH="' .. Utility:Base64Encode(self.project_src_path) .. '"')
		defines('CHAOS_PROJECT_BUILD_PATH="' .. Utility:Base64Encode(targ) .. '"')
		prebuildcommands('{ECHO} CHAOS_PROJECT_PATH       = "' .. self.project_path .. '"')
		prebuildcommands('{ECHO} CHAOS_PROJECT_SRC_PATH   = "' .. self.project_src_path .. '"')
		prebuildcommands('{ECHO} CHAOS_PROJECT_BUILD_PATH = "' .. targ .. '"')
	end

	characterset("ASCII")
end







--------------------------------------------------------------------
-- function to create any kind of project
--------------------------------------------------------------------

function BuildSystem:CppProject(in_kind, proj_type)



	local result = self:AddProject(self.project_name, {
	
	
	
	


	})



	-- the name of the group
	
	local group_name = nil
	if (self.current_group ~= nil) then
		group_name = path.join(self.current_group, self.project_name)
		group(group_name)
	end

	-- create a project for the resources
	local proj_location = path.join(SOLUTION_PATH, self.project_path)
	local res_path = path.join(self.project_src_path, "resources")

	local resource_proj_name = result:GetResourceProjectName()
	project(resource_proj_name)
	kind(SPECIAL_PROJECT)
	location(proj_location)
	files {path.join(res_path, "**")}

	local resource_project = project()

	-- create the project itself
	local proj_location = path.join(SOLUTION_PATH, self.project_path)
	local inc_path = path.join(self.project_src_path, "include")
	local src_path = path.join(self.project_src_path, "src")
	local res_path = path.join(self.project_src_path, "resources")

	project(PROJ_NAME)
	kind(in_kind)
	location(proj_location)
	
	local lua_project = project()

	local result = {
		proj_type = proj_type,
		path = self.project_path,
		root_path = self.project_src_path,
		build_path = self.project_build_path,
		lua_project = lua_project,
		resource_project = resource_project,
		targetdir = GetPlatConfArray({}),
		includedirs = GetPlatConfArray({}),
		group_name = group_name,
		proj_location = proj_location,
		inc_path = GetPlatConfArray(inc_path),
		src_path = GetPlatConfArray(src_path),
		res_path = GetPlatConfArray(res_path)
	}


















	language "C++"

	if (_ACTION == "codelite") then
		cppdialect "C++2a"
	else
		cppdialect "C++20"
	end
	
		--staticruntime "on"

	-- change entry point for windows (avoid WinMain to main)
	if (os.target() == "windows") then
		if (proj_type == ProjectType.EXECUTABLE) then
			entrypoint "mainCRTStartup"
		end
	end

	-- some files including *.cpp, *.c, *.hpp, *.h, *.ixx
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
	filter { }

	-- release/debug settings
	AllTargets(
		function(plat, conf)
			if (conf == DEBUG) then
				self:DebugConf(plat)
			else
				self:ReleaseConf(plat)
			end
			self:OnConfig(in_kind, proj_type, plat, conf, result)
		end
	)

	-- special filter for copying dll
	filter 'files:**.dll'
	  buildmessage 'DLL HANDLING %{file.basename}.dll'
		build_command_str = Utility:QuotationMarks(COPY_SCRIPT, '%{file.abspath}', '%{cfg.targetdir}/%{file.basename}.dll')
		buildcommands(build_command_str)
		buildoutputs '%{cfg.targetdir}/%{file.basename}.dll'



	return result
end

--------------------------------------------------------------------
-- function to create any kind of library
--------------------------------------------------------------------
function BuildSystem:LibraryHelper(kind_library, type)
	local result = selft:CppProject(kind_library, type)
	result.libname = GetPlatConfArray(result.name)
	result:AddFileToCopy("resources")
	if (type == ProjectType.SHARED_LIBRARY) then
		result:AddFileToCopy("@" .. path.join(result.targetdir[x64][DEBUG], result.name .. ".dll"))
	end
	result:GenDoxygen()
	return result
end

--------------------------------------------------------------------
-- declare a static library
--------------------------------------------------------------------
function BuildSystem:StaticLibrary()
	return self:LibraryHelper("StaticLib", ProjectType.STATIC_LIBRARY)
end

--------------------------------------------------------------------
-- declare a shared library
--------------------------------------------------------------------
function BuildSystem:SharedLibrary()
	local result = self:LibraryHelper("SharedLib", ProjectType.SHARED_LIBRARY)
	filter {}
	defines('CHAOS_IS_BUILDING_DLL')
	allmodulespublic "on" -- required for DLL+modules (requires at least premake 5.0.0-beta2)
	return result
end

--------------------------------------------------------------------
-- Create a windowed application
--------------------------------------------------------------------
function BuildSystem:WindowedApp()
	local result = self:CppProject("WindowedApp", ProjectType.EXECUTABLE)
	result:GenZIP()
	return result
end