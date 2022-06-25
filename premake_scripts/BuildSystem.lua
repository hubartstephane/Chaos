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


--[[

	if (not Object::IsNil(tocopy)) then
		DeclareToCopyFile(tocopy, result)
	end

]]--

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






















function WindowedApp()
  
  local result = Project.new()
  
end