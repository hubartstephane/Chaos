Object = {}

function Object:new(data)

  data = data or {}
  setmetatable(data, self)
  self.__index = self
  return data
end

-- returns an ENV array that contains all required members from self
function Object:StoreEnvironment(names, result)
	result = result or {}
	for k, v in pairs(names) do
		result[v] = self[v]
	end
	return result
end

-- take an ENV and write back all members to _G
function Object:RestoreEnvironment(env)
	for k, v in pairs(env) do
		self[k] = v
	end
end
