print("purpose : ")
print(" - Test class creation")
print(" - create 2 classes MYPARENTCLASS and MYCHILDCLASS")
print(" - call methods Func1(...) and Func2(...)")
print(" - override methods Func1(...) but not Func2(...)")
print(" - acces the instance member \"myvalue\" (not defined in the second instance)")
print(" - show that the garbage collect does nothing for non userdata object")
print("")


-- create a class (this is a metatable for its children)
local MYPARENTCLASS = {}
MYPARENTCLASS.__index = MYPARENTCLASS

MYPARENTCLASS.__gc = function ()
  print("Instance beeing __gc : value = " .. tostring(self.myvalue))
end

-- add New function inside the class
function MYPARENTCLASS:New(o)
  o = o or {}
  setmetatable(o, self)      -- self becomes the metatable of the object created
  return o
end

-- add a method for the instances
function MYPARENTCLASS:Func1()
  print("MYPARENTCLASS.Func1() : value = " .. tostring(self.myvalue))
end

function MYPARENTCLASS:Func2()
   print("MYPARENTCLASS.Func2() : value = " .. tostring(self.myvalue))
end

-- create an instance and call a method
local instance1 = MYPARENTCLASS:New()
instance1.myvalue = 666

instance1:Func1()
instance1:Func2()

print("------------------------------")

-- create a child class and override the method Func1(...)  but not Func2(...)
local MYCHILDCLASS = MYPARENTCLASS:New()
MYCHILDCLASS.__index = MYCHILDCLASS


function MYCHILDCLASS:Func1()
  print("MYCHILDCLASS.Func1() : value = " .. tostring(self.myvalue))
end

-- create an instance, call Func1(...) and Func2(...) : ensure the appropriate methods are called
local instance2 = MYCHILDCLASS:New()
instance2:Func1()
instance2:Func2()

print("------------------------------")
print("garbage collector about to be invoked => does nothing while this is not user data")
collectgarbage()