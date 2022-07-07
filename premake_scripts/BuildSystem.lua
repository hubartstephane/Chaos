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
function BuildSystem:DisplayInformation()
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
		
	local upper_name = string.upper(name)
	
	if (self.projects[upper_name]) then
		assert(false, "Project " .. upper_name .. " already definied")
	else
		local result = Project:new(data)
		result.project_name = name
		result.targetdir = result.targetdir or Utility:GetPlatConfArray("")
		result.includedirs = result.includedirs or Utility:GetPlatConfArray({})
		result.additionnal_libs = result.additionnal_libs or Utility:GetPlatConfArray({})
		result.dependencies = {}
		result.tocopy = result.tocopy or Utility:GetPlatConfArray({})
		self.projects[upper_name] = result
		
		return result
	end
end

--------------------------------------------------------------------
-- declare an external library (not described by any premake project)
--------------------------------------------------------------------
function BuildSystem:DeclareExternalLib(external_name, inc_path, lib_path, libname, tocopy)
	
	local result = self:AddProject(external_name, {
		project_type = ProjectType.EXTERNAL_LIBRARY,
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
			if (result.targetdir[plat][conf]) then -- in some cases there may be a library to link with but no directory specified (ex OpenGL)		
				Utility:ForEachElement(result.libname[plat][conf],
					function(libname)
						local fullpath = result.targetdir[plat][conf] .. "/" .. libname
						if not os.isfile(fullpath) then
							assert(false, "library does not exit: " .. fullpath)
						end
					end				
				)
			end			
		end
	)
	return result

end

--------------------------------------------------------------------
-- function to create any kind of project
--------------------------------------------------------------------
function BuildSystem:CppProject(project_type)

	-- basics
	local result = self:AddProject(self.project_name, {
		project_type = project_type,
		current_group = self.current_group,
		project_path = self.project_path,
		project_src_path = self.project_src_path,
		project_build_path = self.project_build_path
	})	

	-- per conf/plat
	Utility:AllTargets(
		function(plat, conf)
			-- where the result EXE/LIB is been saved
			local targ = path.join(result.project_build_path, plat, conf)
			result.targetdir[plat][conf] = targ
			-- where the includes are for the project
			local inc = path.join(result.project_src_path, "include")
			table.insert(result.includedirs[plat][conf], inc)
		end
	)
	
	-- for EXECUTABLE/SHARED LIB/STATIC LIB, always considere copying 'resources'
	result:AddFileToCopy("resources")
	
	return result

end

--------------------------------------------------------------------
-- function to create any kind of library
--------------------------------------------------------------------
function BuildSystem:LibraryHelper(project_type)
	local result = self:CppProject(project_type)
	result.libname = Utility:GetPlatConfArray(result.project_name)
	
	
	-- !!! HERE [x64][DEBUG] copy ???
	
	
	if (project_type == ProjectType.SHARED_LIBRARY) then
		result:AddFileToCopy("@" .. path.join(result.targetdir[x64][DEBUG], result.name .. ".dll")) 
	end
	
	
	result:GenDoxygen() -- automatic documentation for libraries
	return result
end

--------------------------------------------------------------------
-- declare a static library
--------------------------------------------------------------------
function BuildSystem:StaticLibrary()
	return self:LibraryHelper(ProjectType.STATIC_LIBRARY)
end

--------------------------------------------------------------------
-- declare a shared library
--------------------------------------------------------------------
function BuildSystem:SharedLibrary()
	return self:LibraryHelper(ProjectType.SHARED_LIBRARY)
end

--------------------------------------------------------------------
-- Create a windowed application
--------------------------------------------------------------------
function BuildSystem:WindowedApp()
	return self:CppProject(ProjectType.EXECUTABLE)
end

--------------------------------------------------------------------
-- Collect all dependencies
--------------------------------------------------------------------
function BuildSystem:CollectDependencies()

	-- propagate recursively dependencies
	local changes = true
	while changes do
		changes = false
		for _, proj in pairs(self.projects) do
			local count1 = #proj.dependencies
			for _, depend_project_name in ipairs(proj.dependencies) do
				table.append(proj.dependencies, self.projects[depend_project_name].dependencies)
			end				
			local count2 = #proj.dependencies
			if (count2 ~= count1) then
				changes = true
			end
		end		
	end

	-- the array dependencies becomes an array of Projects instead of an array of strings
	for k, proj in pairs(self.projects) do
		table.transform(proj.dependencies,
			function(depend_project_name)
				return self.projects[depend_project_name]
			end
		)
	end

end

--------------------------------------------------------------------
-- Make the solution
--------------------------------------------------------------------

function BuildSystem:MakeSolution()

	solution "Chaos"

	platforms {table.unpack(PLATFORMS)}
	configurations {table.unpack(CONFIGS)}
	
	local arch = _OPTIONS['arch']
	if arch then
		Log:Output("Architecture: " .. arch)
		architecture(arch)
	end
	
	location(SOLUTION_PATH) -- where the visual studio project file is been created

	if os.target() == "windows" then
		defines {"WINDOWS"}
	end

	if os.target() == "linux" then
		defines {"LINUX"}
	end
	
	for k, proj in pairs(self.projects) do
		proj:DisplayInformation()
		proj:AddProjectToSolution()
	end

end