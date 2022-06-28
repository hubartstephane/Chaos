require "Object"

ProjectType = {  
	EXECUTABLE = {},
	STATIC_LIBRARY = {},
	SHARED_LIBRARY = {},
	EXTERNAL_LIBRARY = {},
	RESOURCES = {}    
}

Project = Object:new({
  gen_zip = false,
  gen_doxygen = false  
})

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

-- add a file/directory in the TO_COPY list
function Project:AddFileToCopy(filename)
	local tmp = Utility:GetPlatConfArray(filename)
	Utility:AllTargets(
		function(plat, conf)
			self:AddFileToCopyHelper(tmp[plat][conf], plat, conf)
		end
	)
end

-- require documentation generation
function Project:GenDoxygen()
	self.gen_doxygen = true
end
-- require ZIP generation
function Gen_ZIP()
	self.genzip = true
end


