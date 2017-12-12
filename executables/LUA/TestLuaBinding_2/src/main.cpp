
#include <chaos/LuaTools.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>
#include <chaos/LogTools.h>
#include <chaos/LuaState.h>
#include <chaos/InheritanceIntrospection.h>
#include <chaos/LuaBinding.h>




namespace chaos 
{

	// -------------------------------------------------------------------------------


	// -------------------------------------------------------------------------------

};
















// ---------------------------------------------------------------------------

float F1()
{



	return 6.66f;
}




class A {};

class B : public A {};

class C : public B {};

class D {};

void EnrichLuaState1(chaos::LuaState & L)
{
	chaos::LuaBinding binding(L);

	binding.AddFunction("F1", &F1, 0);
	/*
	binding.GetTOMT();

	ClassBinding C = binding.AddClass<A, B>("A");

	ClassBinding C = binding.AddClass<A, B>("A");
	C.AddMethod("f1", &A::f1);
	C.AddMethod("f2", &A::f2);
	*/

	/*
	chaos::DeclareClass<A>(L, "A");
	chaos::DeclareClass<B, A>(L, "B");
	chaos::DeclareClass<C, B>(L, "C");

	A    * a = chaos::GetStackArgument<A>(L, -1);
	void * v = chaos::GetStackArgument<void>(L, -1);

	a = a;
	*/
}


void WorkWithLua1(chaos::LuaState & L)
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

		chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(p, true);
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

		chaos::LogTools::DisplayTitle("Test1.lua : work with C++ classes");

		StartLuaFile(rp / "test1.lua", WorkWithLua1, EnrichLuaState1);

		chaos::WinTools::PressToContinue();
		return true;
	}
};


int _tmain(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}










#if 0

// -------------------------------------------------------------------------------

bool IsUserDataInstanceOf(LuaState & L, InheritanceIntrospection::ClassRuntimeID expected_ID, int index)
{
	bool result = false;

#if _DEBUG
	ensure_luatop_const(L);
#endif

	if (index != 0)
		index = lua_absindex(L, index);

	if (lua_isuserdata(L, index))
	{
		lua_getmetatable(L, index); // get the MT of the instance
		GetMT<T>(L);         // get expected MT

		GetUse


	}
	return result;
}





// -------------------------------------------------------------------------------










template<typename T>
T * GetStackArgument(LuaState & L, int index)
{
	force_luatop_const(L);

	// translate index as absolute position
	if (index != 0)
		index = lua_absindex(L, index);

	// try conversion
	if (lua_isuserdata(L, index)) // considered object on the stack is user data, good
	{   
		lua_getmetatable(L, index); // gets its MT
		lua_pushstring(L, ID_KEY);  // search  MT["ID"] to get the class ID of considered object
		lua_rawget(L, -2);

		if (lua_isuserdata(L, -1)) // we could ensure it is a std::pair<...> => will considere a userdata as enought (in fact, this test is not really necessary while we created the ID ourselves)
		{
			InheritanceIntrospection::InheritanceClassChain expected_ID = InheritanceIntrospection::GetClassRuntimeID<T>();
			InheritanceIntrospection::InheritanceClassChain object_ID   = *(InheritanceIntrospection::InheritanceClassChain const *)lua_touserdata(L, -1);

			if (InheritanceIntrospection::InheritsFrom(&object_ID, &expected_ID)) // if class is good, returns the userdata
				return (T *)lua_touserdata(L, index);
		}
	} 
	return nullptr;
}





template<>
void * GetStackArgument<void>(LuaState & L, int index)
{
	return nullptr;
}


template<typename BASE>
void DeclareClass(LuaState & L, char const * classname, int index = 0)
{
	assert(classname != nullptr);

	ensure_luatop_const(L);

	InheritanceIntrospection::InheritanceClassChain ID = InheritanceIntrospection::GetClassRuntimeID<BASE>();

	// normalize the index
	if (index != 0)
		index = lua_absindex(L, index);

	// put the TOMT on the stack
	GetTOMT(L);

	int all_mt_index = lua_gettop(L);

	// create the MT of class BASE
	lua_newtable(L);

	int mt_index = lua_gettop(L);

	// register inside the all MT table 
	L.PushUserData(ID);            // KEY
	lua_pushvalue(L, mt_index);    // VALUE = duplicate the MT table
	lua_settable(L, all_mt_index);

	// MT is on the stack => register ID
	lua_pushstring(L, ID_KEY);
	L.PushUserData(ID);
	lua_rawset(L, mt_index);

	// Register __index in MT


	// Register __newindex in MT


	// Register __gc in MT



	// Create the CLASS table  
	lua_newtable(L);              

	int class_index = lua_gettop(L);

	if (index == 0) // register table in global
	{
		lua_pushvalue(L, class_index); // a duplication that will be consumed
		lua_setglobal(L, classname);
	}
	else // register
	{
		lua_pushstring(L, classname);
		lua_pushvalue(L, class_index); // a duplication that will be consumed
		lua_settable(L, index);
	}

	// add the NEW method inside the CLASS table + some static methods


	lua_pop(L, 1); // the CLASS table
	lua_pop(L, 1); // the MT
	lua_pop(L, 1); // the table of all MT
}

template<typename BASE, typename PARENT>
void DeclareClass(LuaState & L, char const * classname, int index = 0)
{
	// know the introspection system knows BASE => PARENT
	InheritanceIntrospection::DeclareInheritance<BASE, PARENT>(); 
	// create evey thing that is required
	DeclareClass<BASE>(L, classname, index);
}
};

#endif












#if 0



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


//------------------------------------------------------------------------

//------------------------------------------------------------------------



//------------------------------------------------------------------------
template<typename FUNC_TYPE>
FUNC_TYPE GetFunctionInClosure(lua_State * state)
{
	assert(state != nullptr);
	int upvalue_index = lua_upvalueindex(1); // get the pseudo index for the first upvalue (closure)
	assert(lua_islightuserdata(state, upvalue_index));
	return (FUNC_TYPE)lua_touserdata(state, upvalue_index);
}

//------------------------------------------------------------------------


class LuaBinding : public chaos::LuaState
{
public:

	/** constructor */
	LuaBinding(lua_State * in_state) : chaos::LuaState(in_state) { assert(in_state != nullptr); }

	template<typename RET>
	void AddFunction(RET (*func)(), char const * function_name, int index = 0)
	{
		ensure_luatop_const(state);
		if (index == 0)
		{
			lua_pushlightuserdata(state, func);
			lua_pushcclosure(state,  &LuaBindFunction<RET>, 1); // push the closure, consums the previous value
			lua_setglobal(state, function_name);
		}
		else
		{
			index = lua_absindex(state, index);
			lua_pushstring(state, function_name);
			lua_pushlightuserdata(state, func);
			lua_pushcclosure(state, &LuaBindFunction<RET>, 1); // push the closure, consums the previous value
			lua_settable(state, index);
		}
	}

	template<typename RET, typename PARAM0>
	void AddFunction(RET (*func)(PARAM0), char const * function_name, int index = 0)
	{
		ensure_luatop_const(state);
		if (index == 0)
		{
			lua_pushlightuserdata(state, func);
			lua_pushcclosure(state,  &LuaBindFunction<RET, PARAM0>, 1); // push the closure, consums the previous value
			lua_setglobal(state, function_name);
		}
		else
		{
			index = lua_absindex(state, index);
			lua_pushstring(state, function_name);
			lua_pushlightuserdata(state, func);
			lua_pushcclosure(state, &LuaBindFunction<RET, PARAM0>, 1); // push the closure, consums the previous value
			lua_settable(state, index);
		}
	}


};



/*



ClassDef & binding.AddClass<MyClass, void>("MyClass", 0);
ClassDef.AddMethod("Func1", &MyClass::Func1);
ClassDef.AddMember("x", &MyClass::x);
*/

}
#endif