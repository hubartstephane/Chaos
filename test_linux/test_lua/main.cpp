#include <stdio.h>
#include <stdlib.h>

//#define LUA_USE_LINUX
//#define LUA_LIB
#include <lua.hpp>

int main(int argc, char **argv)
{
	lua_State * state = lua_newstate(nullptr, nullptr);
	if (state != nullptr)
	{
		printf("hello world\n");


		lua_close(state);		
	}
	return 0;
}
