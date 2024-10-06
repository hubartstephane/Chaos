print("purpose : ")
print("  -test   __index   and   __newindex   metamethods")
print("")

local MYCLASS = {}
MYCLASS.__index = function(self, k)
  print("try to read : ")
  print("  key         = " .. tostring(k))
  print("  type of key = " .. type(k))
end

MYCLASS.__newindex = function(self, k, v)
  print("try to write : ")
  print("  key           = " .. tostring(k))
  print("  type of key   = " .. type(k))
  print("  value         = " .. tostring(v))
  print("  type of value = " .. type(v))  
end

local instance = {}
setmetatable(instance, MYCLASS)

local a = instance.x

instance.x = 13



