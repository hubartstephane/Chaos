#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <lua.hpp>

void * Mylua_Alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
	return realloc(ptr, nsize);
}

int main(int argc, char ** argv)
{	      
	lua_State *L = lua_newstate(&Mylua_Alloc, nullptr);
    luaopen_base(L);             /* opens the basic library */
    luaopen_table(L);            /* opens the table library */
	luaopen_io(L);               /* opens the I/O library */
	luaopen_string(L);           /* opens the string lib. */
	luaopen_math(L);             /* opens the math lib. */
	lua_close(L);
	
	std::cout << "lua code" << std::endl;

	return 0;
}
 