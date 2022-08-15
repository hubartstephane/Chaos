#include <chaos/Chaos.h>

// ---------------------------------------------------------------------------

void EnrichLuaState0(chaos::LuaState & L)
{
	L.Set("xxx", "valeur from c++", 0); // create a global variable with xxx

										// create a table
	lua_newtable(L);
	// register it in global
	lua_pushvalue(L, -1);
	lua_setglobal(L, "mytable");
	// add a field inside
	L.Set("value", true, -1);
	lua_pop(L, 1);



	L.Set(3, "une valeur indexee par integer dans G");
}

void WorkWithLua0(chaos::LuaState & L)
{
	lua_pushboolean(L , true);
	lua_pushstring(L , "string");
	lua_pushnumber(L , 3.14);
	lua_pushnil(L);
	L.DebugOutputStack(std::cout);

	L.Push(999);
	int type = lua_type(L, -1);
	std::cout << " --> " << lua_typename(L, type) << std::endl;
}

// ---------------------------------------------------------------------------

void EnrichLuaState1(chaos::LuaState & L)
{

}

void WorkWithLua1(chaos::LuaState & L)
{
	std::string my_string;
	if (L.Get("my_string", my_string))
		std::cout << "string found : " << my_string << std::endl;

	lua_Integer my_int;
	if (L.Get("my_int", my_int))
		std::cout << "integer found : " << my_int << std::endl;

	lua_Number my_number;
	if (L.Get("my_float", my_number))
		std::cout << "number found : " << my_number << std::endl;

	bool my_bool;
	if (L.Get("my_bool", my_bool))
		std::cout << "boolean found  : " << my_bool << std::endl;


	// put the table named 'un_tableau' on the stack
	L.GetTable("un_tableau");

	lua_Integer another_number1;
	if (L.Get("troisieme", another_number1, -1))
		std::cout << "troisieme found  : " << another_number1 << std::endl;

	lua_Integer another_number2;
	if (L.Get(17, another_number2, -1))
		std::cout << "_G[17]  : " << another_number2 << std::endl;

	lua_pop(L, 1); // remove the table

	L.DebugOutputStack(std::cout);
}

// ---------------------------------------------------------------------------

int myCFunc(lua_State * state)
{
	int arg_count = lua_gettop(state);

	lua_Integer p1, p2;

	if (lua_isinteger(state, 1) && lua_isinteger(state, 2))
	{
		p1 = lua_tointeger(state, 1); // parameter 1
		p2 = lua_tointeger(state, 2); // parameter 2

		lua_pushinteger(state, 666); // return value 1
		lua_pushinteger(state, 667); // return value 2
		return 2; // number of result
	}
	return 0;
}

int myCFuncClosure(lua_State * state)
{
	int upvalue_index = lua_upvalueindex(1); // get the pseudo index for the first upvalue (closure)

	if (lua_isinteger(state, upvalue_index))
	{
		lua_Integer closure = lua_tointeger(state, upvalue_index);
		// increment the upvalue for next call
		lua_pushinteger(state, closure + 1);
		lua_replace(state, upvalue_index);
		// the return value
		lua_pushinteger(state, closure);
		return 1;
	}
	return 0;
}

void EnrichLuaState2(chaos::LuaState & L)
{
	// test one simple function that returns 2 integers
	lua_pushcfunction(L, myCFunc);
	lua_setglobal(L, "myCFunc");

	// test one closure (1 integer is captured by the function). This integer is returned
	lua_pushinteger(L, 17);
	lua_pushcclosure(L, myCFuncClosure, 1);
	lua_setglobal(L, "myCFuncClosure");
}

void WorkWithLua2(chaos::LuaState & L)
{

}

// ---------------------------------------------------------------------------

void EnrichLuaState3(chaos::LuaState & L)
{

}

void WorkWithLua3(chaos::LuaState & L)
{
	std::cout << "Should never be displayed (because lua_pcall(...) should returns ERRORCODE) !!! " << std::endl;
	assert(0);
}

// ---------------------------------------------------------------------------

void EnrichLuaState4(chaos::LuaState & L)
{

}

void WorkWithLua4(chaos::LuaState & L)
{
	std::cout << "Should never be displayed (because lua_pcall(...) should returns ERRORCODE) !!! " << std::endl;
	assert(0);
}

// ---------------------------------------------------------------------------

void EnrichLuaState5(chaos::LuaState & L)
{


}

void WorkWithLua5(chaos::LuaState & L)
{
#if _DEBUG
	ensure_luatop_const(L);
#endif

	lua_pushcfunction(L, chaos::LuaTools::DefaultErrorFunction);
	int error = lua_gettop(L);

	lua_getglobal(L, "LuaFunction"); // the function on the stack is consumed by the pcall(..) ... but not the error handler
	if (lua_isfunction(L, -1))
	{
		lua_pushinteger(L, 1);
		lua_pushinteger(L, 2);
		lua_pcall(L, 2, 1, error);
	}

	// read the result
	if (lua_isinteger(L, -1))
	{
		lua_Integer result = lua_tointeger(L, -1);
		std::cout << "result = " << result << std::endl;
	}
	lua_pop(L, 1); // remove the result

				   // remove the error function from the stack
	assert(lua_tocfunction(L, error) == chaos::LuaTools::DefaultErrorFunction); // the error function is still on the stack
	lua_pop(L, 1);

	L.DebugOutputStack(std::cout); // display nothing
}

// ---------------------------------------------------------------------------

int func1(lua_State * state)
{
	std::cout << "func1(...)" << std::endl;
	return 0;
}

int func2(lua_State * state)
{
	std::cout << "func2(...)" << std::endl;
	return 0;
}

luaL_Reg myfunctions[] = {
	"func1", func1,
	"func2", func2,
	nullptr, nullptr
};

int func3(lua_State * state)
{
	std::cout << "func3(...)" << std::endl;
	return 0;
}

int func4(lua_State * state)
{
	std::cout << "func4(...)" << std::endl;
	return 0;
}

luaL_Reg myfunctions1[] = {
	"func1", func1,
	"func2", func2,
	nullptr, nullptr
};

luaL_Reg myfunctions2[] = {
	"func3", func3,
	"func4", func4,
	nullptr, nullptr
};


void EnrichLuaState6(chaos::LuaState & L)
{
#if _DEBUG
	int old_top = lua_gettop(L);
#endif

	L.OpenLibs(myfunctions1, 0);
	assert(old_top == lua_gettop(L));

	lua_newtable(L);             // create a table
	lua_pushvalue(L, -1);        // duplicate it for later
	lua_setglobal(L, "myfuncs"); // make the table available has a global variable

	L.OpenLibs(myfunctions2, -1); // still one duplication of the newtable ...
	lua_pop(L, 1);                // ... removed finally here

	assert(old_top == lua_gettop(L));
}

void WorkWithLua6(chaos::LuaState & L)
{



}


// ---------------------------------------------------------------------------

void StartLuaFile(boost::filesystem::path const & p, void (*WorkWithLua)(chaos::LuaState &), void (*EnrichLuaState)(chaos::LuaState &))
{
	lua_State * state = chaos::LuaTools::CreateStandardLuaState();
	if (state != nullptr)
	{
		chaos::LuaState L(state);

		EnrichLuaState(L);

		chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(p, chaos::LoadFileFlag::ASCII);
		if (buffer)
		{
			if (chaos::LuaTools::ExecBuffer(state, buffer, false) == 0)
				WorkWithLua(L);
		}
		lua_close(state);
	}
}




class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		boost::filesystem::path const & rp = GetResourcesPath();

		chaos::Log::Title("Test0.lua : miscellaneous");
		StartLuaFile(rp / "test0.lua", WorkWithLua0, EnrichLuaState0);

		chaos::Log::Title("Test1.lua : miscellaneous");
		StartLuaFile(rp / "test1.lua", WorkWithLua1, EnrichLuaState1);

		chaos::Log::Title("Test2.lua : calling C++ function from LUA");
		StartLuaFile(rp / "test2.lua", WorkWithLua2, EnrichLuaState2);

		chaos::Log::Title("Test3.lua : a syntax error");
		StartLuaFile(rp / "test3.lua", WorkWithLua3, EnrichLuaState3);

		chaos::Log::Title("Test4.lua : a runtime error");
		StartLuaFile(rp / "test4.lua", WorkWithLua4, EnrichLuaState4);

		chaos::Log::Title("Test5.lua : calling LUA function from C++");
		StartLuaFile(rp / "test5.lua", WorkWithLua5, EnrichLuaState5);

		chaos::Log::Title("Test6.lua : OpenLibs");
		StartLuaFile(rp / "test6.lua", WorkWithLua6, EnrichLuaState6);

		chaos::WinTools::PressToContinue();

		return true;
	}
};

int main(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


