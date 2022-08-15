
#include <chaos/Chaos.h>

class MyClass
{
public:

	MyClass(char const * in_name) : x(666) , name(in_name)
	{
		std::cout << "MyClass constructor : " << name << std::endl;
	}
	~MyClass()
	{
		std::cout << "MyClass destructor : "  << name << std::endl;
	}

	void Func1(int value)
	{
		x = value;
		std::cout << name << ".Func1(...) x = " << x << std::endl;
	}

	int x;

	std::string name;
};


int MyClass_Func1Binding(lua_State * state);








MyClass * MyClass_CheckOnStack(lua_State * state, int idx)
{
	MyClass * result = nullptr;
	if (lua_isuserdata(state, idx))
	{
		lua_getmetatable(state, idx);
		luaL_getmetatable(state, "MyClass");
		int equal = lua_rawequal(state, -1, -2);
		lua_pop(state, 2);
		if (equal)
			result = (MyClass *)lua_touserdata(state, idx);
	}
	return result;
}

int MyClass_IndexFunction(lua_State * state)
{
	MyClass * self = MyClass_CheckOnStack(state, 1);
	if (self == nullptr)
		return 0;

	if (!lua_isstring(state, 2))
		return 0;

	char const * member_name = lua_tostring(state, 2);
	if (member_name == nullptr)
		return 0;

	if (strcmp(member_name, "x") == 0)
	{
		lua_pushinteger(state, self->x);
		return 1;
	}

	if (strcmp(member_name, "Func1") == 0)
	{
		lua_pushcfunction(state, MyClass_Func1Binding);
		return 1;
	}

	return 0;
}

int MyClass_NewIndexFunction(lua_State * state)
{
	MyClass * self = MyClass_CheckOnStack(state, 1);
	if (self == nullptr)
		return 0;

	if (!lua_isstring(state, 2))
		return 0;

	char const * member_name = lua_tostring(state, 2);
	if (member_name == nullptr)
		return 0;

	if (strcmp(member_name, "x") == 0)
	{
		if (!lua_isinteger(state, 3))
			return 0;
		self->x = (int)lua_tointeger(state, 3);
	}

	return 0;
}

int MyClass_Func1Binding(lua_State * state)
{
	// get pointer "this"
	MyClass * self = MyClass_CheckOnStack(state, 1);
	if (self == nullptr)
		return 0;

	// get function parameter
	if (!lua_isinteger(state, 2))
		return 0;

	// call the function
	self->Func1((int)lua_tointeger(state, 2));

	return 0;
}

int MyClass_NewBinding(lua_State * state)
{
	assert(state != nullptr);

	// search the metatable
	luaL_getmetatable(state, "MyClass");
	if (!lua_istable(state, -1))
		return 0;
	int MT = lua_gettop(state);

	// read construction parameter : a string
	if (!lua_isstring(state, 1))
		return 0;
	char const * name = lua_tostring(state, 1);
	if (name == nullptr)
		return 0;

	// allocate a userdata
	MyClass * data = (MyClass *)lua_newuserdata(state, sizeof(MyClass));
	if (data == nullptr)
		return 0;
	int user_data = lua_gettop(state);

	// set the metatable to the result
	new (data) MyClass(name);         // inplace allocation
	lua_pushvalue(state, MT);         // duplicate MT on top
	lua_setmetatable(state, user_data);
	return 1;
}

int MyClass_GC(lua_State * state)
{
	MyClass * self = MyClass_CheckOnStack(state, 1);
	if (self == nullptr)
		return 0;

	self->~MyClass();

	return 0;
}









// ---------------------------------------------------------------------------

void EnrichLuaState1(chaos::LuaState & state)
{
	assert(lua_gettop(state) == 0);

	// create a table to contains the New operator named "MyClass" in global namespace
	lua_newtable(state);

	int MyClassTable = lua_gettop(state);

	lua_pushvalue(state, MyClassTable); // duplicate the table and consums it with lua_setglobal
	lua_setglobal(state, "MyClass");

	// declare operator New
	lua_pushstring(state, "New");
	lua_pushcfunction(state, MyClass_NewBinding);
	lua_settable(state, MyClassTable);


	// Normal Metatable (index = 1)
	luaL_newmetatable(state, "MyClass");

	int MT = lua_gettop(state);

	lua_pushstring(state, "__index");
	lua_pushcfunction(state, MyClass_IndexFunction); // Read method
	lua_settable(state, MT);

	lua_pushstring(state, "__newindex");
	lua_pushcfunction(state, MyClass_NewIndexFunction); // Write method
	lua_settable(state, MT);

	lua_pushstring(state, "__gc");
	lua_pushcfunction(state, MyClass_GC); // Destructor
	lua_settable(state, MT);





	lua_pop(state, 1); // pop MT
	lua_pop(state, 1); // pop MyClassTable



#if 0

					   // XXX :
					   //   you cannot use nested Metatable because the __index function would receive as SELF a table and not a userdata
					   //
					   //   instance (USERDATA)
					   //      |
					   //      +---> METATABLE1    => __index function receives (USERDATA, key)
					   //                |
					   //                +---> METATABLE2     => but here __index function receives (METATABLE1, key) => USERDATA information is lost (cannot use this for MEMBER binding)
					   //
					   //



					   // Metatable for Member access (index = 1)
	luaL_newmetatable(state, "MyClass_Member");

	int MemberMT = lua_gettop(state);

	lua_pushstring(state, "__index");
	lua_pushcfunction(state, MyClass_IndexFunction); // Read method
	lua_settable(state, MemberMT);

	lua_pushstring(state, "__newindex");
	lua_pushcfunction(state, MyClass_NewIndexFunction); // Write method
	lua_settable(state, MemberMT);

	// Normal Metatable (index = 2)
	luaL_newmetatable(state, "MyClass");

	int MT = lua_gettop(state);

	lua_pushstring(state, "__index"); // Normal MT.__index points to itself
	lua_pushvalue(state, MT);
	lua_settable(state, MT);

	// Register Func1 binding
	lua_pushstring(state, "Func1");
	lua_pushcfunction(state, MyClass_Func1Binding);
	lua_settable(state, MT);

	lua_pushvalue(state, MemberMT); // Normal MT use MemberMT for data access (after other initialisation)
	lua_setmetatable(state, MT);

	// create a table to contains the New operator named "MyClass" in global namespace
	lua_newtable(state);

	int MyClassTable = lua_gettop(state);

	lua_pushvalue(state, MyClassTable); // duplicate the table and consums it with lua_setglobal
	lua_setglobal(state, "MyClass");

	// declare operator New
	lua_pushstring(state, "New");
	lua_pushcfunction(state, MyClass_NewBinding);
	lua_settable(state, MyClassTable);

	lua_pop(state, 1); // pop MyClassTable
	lua_pop(state, 1); // pop MT
	lua_pop(state, 1); // pop Member MT

#endif
}

void WorkWithLua1(chaos::LuaState & state)
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
			if (chaos::LuaTools::ExecBuffer(L, buffer, false) == 0)
				WorkWithLua(L);
		}
		lua_close(L);
	}
}



class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		boost::filesystem::path const & rp = GetResourcesPath();

		chaos::Log::Title("Test1.lua : work with C++ classes");

		StartLuaFile(rp / "test1.lua", WorkWithLua1, EnrichLuaState1);

		chaos::WinTools::PressToContinue();

		return true;
	}
};



int main(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


