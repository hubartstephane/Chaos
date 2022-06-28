require "Object"
require "Utility"

BuildSystem = Object:new({
	current_group = "",
	project_name = "",
	project_path = "",
	project_src_path = "",
	project_build_path = "",
	projects = {}
})

function BuildSystem:DisplayRootEnvironment()
	if (DISPLAY_ROOT_ENVIRONMENT) then
		Log:Output("=======================================================================")
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
		Log:Output("SCRIPT             : " .. _SCRIPT)
		Log:Output("PROJ_NAME          : " .. self.project_name)
		Log:Output("PROJECT_PATH       : " .. self.project_path)
		Log:Output("PROJECT_SRC_PATH   : " .. self.project_src_path)
		Log:Output("PROJECT_BUILD_PATH : " .. self.project_build_path)
		Log:Output("=======================================================================")
	end
end

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

function BuildSystem:AddProject(name, data)
	
	if (self.projects[name]) then
		assert(false, "Project " .. name .. " already definied")
	else
		local result = Project:new(data)
		self.projects[name] = result
		return result
	end
end

function BuildSystem:DeclareExternalLib(external_name, inc_path, lib_path, libname, tocopy)

	external_name = string.upper(external_name)
	
	Log:Output("DeclareExternalLib [" .. external_name .. "]")
	
	local result = self:AddProject(external_name, {
		name = external_name,
		proj_type = ProjectType.EXTERNAL_LIBRARY,
		root_path = EXTERNAL_PATH,
		includedirs = Utility:PrefixPathArray(Utility:GetPlatConfArray(inc_path), EXTERNAL_PATH),
		targetdir = Utility:PrefixPathArray(Utility:GetPlatConfArray(lib_path), EXTERNAL_PATH),
		libname = Utility:GetPlatConfArray(libname),
		additionnal_libs = Utility:GetPlatConfArray({}),
		dependencies = {},
		tocopy = Utility:GetPlatConfArray({})
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

function Project:DependOnLib(libnames)
	Utility:ForEachElement(libnames,
		function(libname)
			table.insert(self.dependencies, string.upper(libname))
		end
	)
end

function Project:DependOnStandardLib(libname)
	if os.target() ~= "windows" then
		return
	end

	Utility::ForEachElement(libname,
		function(lib)
			Utility::AllTargets(
				function(plat, conf)
					table.insert(self.additionnal_libs[plat][conf], libname)
				end
			)
		end
	)
end














function BuildSystem:WindowedApp()
	--local result = self:CppProject("WindowedApp", ProjectType.EXECUTABLE)
	self:DisplayEnvironment()
	--result:GenZIP()
	--return result
end



















function WindowedApp()
  
  local result = Project.new()
  
end