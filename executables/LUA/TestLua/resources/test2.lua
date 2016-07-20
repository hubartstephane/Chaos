local x, y = myCFunc(17, 18)

print("myCFunc() = " .. x .. " , " .. y)

local z
z = myCFuncClosure()
print("call 1 myCFuncClosure() = " .. z)
z = myCFuncClosure()
print("call 2 myCFuncClosure() = " .. z)
z = myCFuncClosure()
print("call 3 myCFuncClosure() = " .. z)


