
require 'Object'

Utility = Object:new()

function Utility:IsBool(value)
	return (type(value) == "boolean")
end

function Utility:IsTable(value)
	return (type(value) == "table")
end

function Utility:IsNil(value)
	return (type(value) == "nil")
end

function Utility:IsString(value)
	return (type(value) == "string")
end

function Utility:IsNumber(value)
	return (type(value) == "number")
end

function Utility:ForEachElement(src, fun)
	if not self:IsNil(src) then
		if self:IsTable(src) then
			for k, v in pairs(src) do
				fun(v)
			end
		else
			fun(src)
		end
	end
end

-- returns true whether value is a table with PLATFORM/CONFIGS key
function Utility:HasCategoryKey(src, categories)
	if (not self:IsTable(src)) then
		return false
	end

	for k, category in pairs(categories) do
		if (not self:IsNil(src[category])) then
			return true
		end
	end
	return false
end

-- returns true whether value is a table with PLATFORM key
function Utility:HasPlatformKey(src)
	return self:HasCategoryKey(src, PLATFORMS)
end

-- returns true whether value is a table with CONFIG key
function Utility:HasConfigKey(src)
	return self:HasCategoryKey(src, CONFIGS)
end

-- if input is an array, returns a copy of it. return the input unchanged elsewhere
function Utility:DeepCopy(value)
	if (self:IsTable(value)) then
			return table.deepcopy(value)
	end
	return value
end

-- if the input is not an array return it unchanged
-- elsewhere, read in that array the value corresponding to PLATFORM/CONFIG
function Utility:ReadValueFromPlatformConfigArray(value, plat, conf)
	if (self:HasPlatformKey(value)) then
		if (self:HasConfigKey(value[plat])) then
			return value[plat][conf]
		end
		return value[plat]
	elseif (self:HasConfigKey(value)) then
		if (self:HasPlatformKey(value[conf])) then
			return value[conf][plat]
		end
		return value[conf]
	end
	return value
end

-- transform a value (array or not) in the form of [PLATFORM][CONFIG]
function Utility:GetPlatConfArray(value)
	local result = {}
	for k, platform in pairs(PLATFORMS) do
		result[platform] = {}
		for k, config in pairs(CONFIGS) do
			result[platform][config] = self:DeepCopy(self:ReadValueFromPlatformConfigArray(value, platform, config))
		end
	end
	return result
end

-- add a prefix to a path (the path is a table[PLATFORM][CONFIG] format)
function Utility:PrefixPathArray(src, prefix)
	self:AllTargets(
		function(plat, conf)
			if (src[plat][conf]) then
				src[plat][conf] = path.join(prefix, src[plat][conf])
			end
		end
	)
	return src
end

function Utility:AllTargets(fun)
	for k, platform in pairs(PLATFORMS) do
		for k, config in pairs(CONFIGS) do
			fun(platform, config)
		end
	end
end