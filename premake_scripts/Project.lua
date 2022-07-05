require "Object"

--------------------------------------------------------------------
-- The types of projects
--------------------------------------------------------------------
ProjectType = {
	EXECUTABLE = "WindowedApp",
	STATIC_LIBRARY = "StaticLib",
	SHARED_LIBRARY = "SharedLib",
	EXTERNAL_LIBRARY = {},
	RESOURCES = "Makefile"
}

--------------------------------------------------------------------
-- Class declaration
--------------------------------------------------------------------
Project = Object:new({
	gen_zip = false,
	gen_doxygen = false
})

--------------------------------------------------------------------
-- Display project information
--------------------------------------------------------------------

function Project:DisplayInformation()
	Log:Output("=======================================================================")
	Log:Output("PROJECT_NAME       : " .. self.project_name)
	Log:Output("PROJECT_PATH       : " .. self.project_path)
	Log:Output("PROJECT_SRC_PATH   : " .. self.project_src_path)
	Log:Output("PROJECT_BUILD_PATH : " .. self.project_build_path)
	Log:Output("CURRENT_GROUP      : " .. self.current_group)
	Log:Output("=======================================================================")
end

--------------------------------------------------------------------
-- for file copying (from src to build directory),
-- some paths start with @ -> in that case, the file is copied in the build directory directly (no matter what SRC relative path is) (useful for DLL)
--
--		src/toto/titi/file.txt => build/file.txt
--
-- some does not -> they are copied in the equivalent path
--
-- 		src/toto/titi/file.txt => build/toto/titi/file.txt
--
-- TO_COPY is an array of {dst_path, src_path}

-- add input (whether it is a string or a table) into TO_COPY
--------------------------------------------------------------------
function Project:AddFileToCopyHelper(filenames, plat, conf)
	Utility:ForEachElement(filenames,
		function(filename)
			local src_path = ""
			if (string.sub(filename, 1, 1) == "@") then -- the file is to be copied directly in the same directory than the executable itself
				filename = string.sub(filename, 2)
				src_path = path.join(self.root_path, filename)
				filename = path.getname(filename) -- remove the path before the filename
			else
				src_path = path.join(self.root_path, filename)
			end
			table.insert(self.tocopy[plat][conf], {filename, src_path})
		end
	)
end

--------------------------------------------------------------------
-- add a file/directory in the TO_COPY list
--------------------------------------------------------------------
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
-- gets the name of the premake project for documentation
--------------------------------------------------------------------
function Project:GetDocumentationProjectName()
	return "Documentation_of_" .. self.proj_name
end

--------------------------------------------------------------------
-- gets the name of the premake project for ZIP
--------------------------------------------------------------------
function Project:GetZipProjectName()
	return "ZIP_of_" .. self.proj_name
end

--------------------------------------------------------------------
-- gets the name of the premake project for resources
--------------------------------------------------------------------
function Project:GetResourceProjectName()
	return "Resources_of_" .. self.proj_name
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

function Project:DebugConf(plat)
	filter {"configurations:" .. DEBUG, "platforms:" .. plat}
	defines {DEBUG}
	defines {"_DEBUG"}
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
	optimize "On"
	symbols "Off"
	flags {"MultiProcessorCompile"}
end

--------------------------------------------------------------------
-- generic initializations
--------------------------------------------------------------------

function Project:OnConfig(plat, conf)

	Log:Output("X")

	targetdir(self.targetdir[plat][conf])
	includedirs(self.includedirs[plat][conf])
	-- some definition for FILE REDIRECTION
	if (self.proj_type == ProjectType.EXECUTABLE) then
		defines('CHAOS_PROJECT_PATH="' .. Utility:Base64Encode(self.project_path) .. '"')
		defines('CHAOS_PROJECT_SRC_PATH="' .. Utility:Base64Encode(self.project_src_path) .. '"')
		defines('CHAOS_PROJECT_BUILD_PATH="' .. Utility:Base64Encode(self.targetdir[plat][conf]) .. '"')
		prebuildcommands('{ECHO} CHAOS_PROJECT_PATH       = "' .. self.project_path .. '"')
		prebuildcommands('{ECHO} CHAOS_PROJECT_SRC_PATH   = "' .. self.project_src_path .. '"')
		prebuildcommands('{ECHO} CHAOS_PROJECT_BUILD_PATH = "' .. self.proj.targetdir[plat][conf] .. '"')
	end	
	
		Log:Output("Y")
end




--[[



	-- where the result EXE/LIB is been saved
	local targ = path.join(self.project_build_path, plat, conf)
	targetdir(targ)
	self.targetdir[plat][conf] = targ

	-- where the includes are for the project
	local inc = path.join(self.project_src_path, "include")
	includedirs(inc)
	self.includedirs[plat][conf] = inc

end

]]--

--------------------------------------------------------------------
-- Create a sub-premake project for ZIP
--------------------------------------------------------------------
function Project:AddZipProjectToSolution()

	-- only if requested
	if (not self.gen_zip) then
		return
	end


end

--------------------------------------------------------------------
-- Create a sub-premake project for DOC
--------------------------------------------------------------------
function Project:AddZipProjectToSolution()

	-- only if requested
	if (not self.gen_documentation) then
		return
	end
	
	

end

--------------------------------------------------------------------
-- Add the project into premake solution
--------------------------------------------------------------------

function Project:AddProjectToSolution()

	-- ignore external project
	if (self.proj_type == ProjectType.EXTERNAL_LIBRARY) then
		return
	end
	
	-- declare project
	project(self.proj_name)
	
	
	
	
		Log:Output("X")	
	-- kind		
	kind(self.proj_type)		
	if (self.proj_type == ProjectType.SHARED_LIBRARY) then
		defines('CHAOS_IS_BUILDING_DLL')
		allmodulespublic "on" -- required for DLL+modules (requires at least premake 5.0.0-beta2)		
	end
	
		Log:Output("X2")
	
	-- language/dialect
	language "C++"

	if (_ACTION == "codelite") then
		cppdialect "C++2a"
	else
		cppdialect "C++20"
	end
	
		Log:Output("X3")
	
	-- entry point (avoid WinMain to main)
	if (os.target() == "windows") then
		if (proj_type == ProjectType.EXECUTABLE) then
			entrypoint "mainCRTStartup"
		end
	end

	-- files to handle: *.cpp, *.c, *.hpp, *.h, *.ixx
	local src_h = path.join(self.project_src_path, "**.h")
	local src_hpp = path.join(self.project_src_path, "**.hpp")
	local src_c = path.join(self.project_src_path, "**.c")
	local src_cpp = path.join(self.project_src_path, "**.cpp")
	local src_ixx = path.join(self.project_src_path, "**.ixx")
	files {src_h, src_hpp, src_c, src_cpp, src_ixx}

	Log:Output("X4")

	-- handle C++ modules
	filter {"files:**.ixx" }
		buildaction "ClCompile"
		compileas "Module"
	filter {}

	Log:Output("X5")

	-- release/debug settings
	AllTargets(
		function(plat, conf)
			if (conf == DEBUG) then
				self:DebugConf(plat)
			else
				self:ReleaseConf(plat)
			end

		end
	)
	filter {}
		
	Log:Output("X6")		
		
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

	-- special
	self.AddZipProjectToSolution()
	self.AddDocProjectToSolution()
	
end










--[[





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

	---------------------------------
	-- IMPORTANT
	---------------------------------	
	-- path = self.project_path,
	-- root_path = self.project_src_path,
	-- build_path = self.project_build_path,
	---------------------------------	



	local result = {
		proj_type = proj_type,
		path = self.project_path,
		root_path = self.project_src_path,
		build_path = self.project_build_path,
		
		

		targetdir = GetPlatConfArray({}),
		includedirs = GetPlatConfArray({}),
		group_name = group_name,
		proj_location = proj_location,
		inc_path = GetPlatConfArray(inc_path),
		src_path = GetPlatConfArray(src_path),
		res_path = GetPlatConfArray(res_path)
	}


		lua_project = lua_project,
		resource_project = resource_project,
















	return result
end


]]--
