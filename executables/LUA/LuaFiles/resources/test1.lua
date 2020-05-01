print("purpose : ")
print(" - Access variable environment")
print("")


print(" indirect creation of a global variable : ")
_G["toto"] = "truc"
print("toto = " .. toto)


print(" Yes, we can create a variable whose name is an integer ! (but its difficult to access this without _G[...] construction : ")
_G[17] = "17 en string"

print(" List of all global variables (see 17)")
for n in pairs(_G) do 
  print(" - " .. n) 
end

