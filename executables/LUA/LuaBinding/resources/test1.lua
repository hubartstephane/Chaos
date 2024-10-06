-- allocate new userdata object
local myinstance = MyClass.New("instance1")
-- call a function on the instance (change internal state of x)
myinstance:Func1(17)

-- try read member 
local x = myinstance.x
-- try write member
myinstance.x = 777


function G()
  local myotherinstance = MyClass.New("instance2") -- should trigger a GarbageCollect at function exit
end

print("G() creates an instance of MyClass : Expected result => should be destroyed at function exit")
print("")
G()
print("")
print("Well, local instance is not destroyed yet !!")
print("")
print("Now that the script is about to end, all instances of MyClass should be collected")
