#include <stdio.h>
#include <stdlib.h>

//#define GLFW_DLL
#include <GLFW/glfw3.h>

//#define LUA_USE_LINUX
//#define LUA_LIB
#include <lua.hpp>

int main(int argc, char **argv)
{


	glfwInit();

#if 0
	lua_State * state = lua_newstate(nullptr, nullptr);
	if (state != nullptr)
	{
		printf("hello world\n");


		lua_close(state);		
	}
#endif
	return 0;
}
