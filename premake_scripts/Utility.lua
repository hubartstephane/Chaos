
require 'Object'

--------------------------------------------------------------------
-- Class declaration
--------------------------------------------------------------------
Utility = Object:new()

--------------------------------------------------------------------
-- check whether the src is a bool
--------------------------------------------------------------------
function Utility:IsBool(value)
	return (type(value) == "boolean")
end

--------------------------------------------------------------------
-- check whether the src is a table
--------------------------------------------------------------------
function Utility:IsTable(value)
	return (type(value) == "table")
end

--------------------------------------------------------------------
-- check whether the src is nil
--------------------------------------------------------------------
function Utility:IsNil(value)
	return (type(value) == "nil")
end

--------------------------------------------------------------------
-- check whether the src is a string
--------------------------------------------------------------------
function Utility:IsString(value)
	return (type(value) == "string")
end

--------------------------------------------------------------------
-- check whether the src is a number
--------------------------------------------------------------------
function Utility:IsNumber(value)
	return (type(value) == "number")
end

--------------------------------------------------------------------
-- call the callback for all elements of src if it is a table, or on src elsewhere
--------------------------------------------------------------------
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

--------------------------------------------------------------------
-- returns true whether value is a table with PLATFORM/CONFIGS key
--------------------------------------------------------------------
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

--------------------------------------------------------------------
-- returns true whether value is a table with PLATFORM key
--------------------------------------------------------------------
function Utility:HasPlatformKey(src)
	return self:HasCategoryKey(src, PLATFORMS)
end

--------------------------------------------------------------------
-- returns true whether value is a table with CONFIG key
--------------------------------------------------------------------
function Utility:HasConfigKey(src)
	return self:HasCategoryKey(src, CONFIGS)
end

--------------------------------------------------------------------
-- if input is an array, returns a copy of it. return the input unchanged elsewhere
--------------------------------------------------------------------
function Utility:DeepCopy(value)
	if (self:IsTable(value)) then
			return table.deepcopy(value)
	end
	return value
end

--------------------------------------------------------------------
-- if the input is not an array return it unchanged
-- elsewhere, read in that array the value corresponding to PLATFORM/CONFIG
--------------------------------------------------------------------
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

--------------------------------------------------------------------
-- transform a value (array or not) in the form of [PLATFORM][CONFIG]
--------------------------------------------------------------------
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

--------------------------------------------------------------------
-- add a prefix to a path (the path is a table[PLATFORM][CONFIG] format)
--------------------------------------------------------------------
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

--------------------------------------------------------------------
-- apply all combinaison of platform/config to a given callback
--------------------------------------------------------------------
function Utility:AllTargets(fun)
	for k, platform in pairs(PLATFORMS) do
		for k, config in pairs(CONFIGS) do
			fun(platform, config)
		end
	end
end

--------------------------------------------------------------------
-- output a string representation of any object
--------------------------------------------------------------------
function Utility:GetDebugString(obj)
	if (self:IsBool(obj)) then
		if (obj) then
			return "true"
		else
			return "false"
		end
	end
	if (self:IsNil(obj)) then
		return "nil"
	end
	if (self:IsNumber(obj)) then
		return tostring(obj)
	end
	if (self:IsString(obj)) then
		return '"' .. obj .. '"'
	end
	if (self:IsTable(obj)) then
		local result = "{"
		local first = true
		for k, v in pairs(obj) do
			if (not first) then
				result = result .. ", "
			end
			result = result .. self:GetDebugString(k) .. " => " .. self:GetDebugString(v)
			first = false
		end
		result = result .. "}"
		return result
	end
	return "???"
end

--------------------------------------------------------------------
-- concat some strings wrapping them with "
--------------------------------------------------------------------
function Utility:QuotationMarks(...)
	local arg = {...}

	local result = ""
	for k, v in pairs(arg) do
		if (result ~= "") then
			result = result .. " "
		end
		result = result .. '"' .. tostring(v) .. '"'
	end
	return result
end

--------------------------------------------------------------------
-- encode a string into a base 64 (code coming from internet)
--------------------------------------------------------------------

function Utility:Base64Encode(data)
	local b = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

	return ((data:gsub(
		".",
		function(x)
			local r, b = "", x:byte()
			for i = 8, 1, -1 do
				r = r .. (b % 2 ^ i - b % 2 ^ (i - 1) > 0 and "1" or "0")
			end
			return r
		end
		) .. "0000"):gsub(
			"%d%d%d?%d?%d?%d?",
			function(x)
				if (#x < 6) then
					return ""
				end
				local c = 0
				for i = 1, 6 do
					c = c + (x:sub(i, i) == "1" and 2 ^ (6 - i) or 0)
				end
				return b:sub(c + 1, c + 1)
			end
		) .. ({"", "==", "="})[#data % 3 + 1])
end

--------------------------------------------------------------------
-- extend table
--------------------------------------------------------------------
function table.find(t, elem)
  for _, v in ipairs(t) do
    if (v == elem) then
      return true
    end
  end
  return false
end

function table.append(t, other_table)
	for _, v in ipairs(other_table) do
		if (not table.find(t, v)) then
			table.insert(t, v)
		end
	end
end
