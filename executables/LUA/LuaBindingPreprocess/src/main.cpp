
#include <chaos/LuaTools.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>
#include <chaos/LuaState.h>




#define CHAOS_LUABIND_MAXFUNC_PARAMETERS 10




#if 0

template<typename T>
struct luabindtype : public boost::mpl::identity<T> {};

template<>
struct luabindtype<int> : public boost::mpl::identity<lua_Integer> {};

template<>
struct luabindtype<float> : public boost::mpl::identity<lua_Number> {};
template<>
struct luabindtype<double> : public boost::mpl::identity<lua_Number> {};




void GetArgument(lua_State * state, int stack_index, int * result)
{
	assert(state      != nullptr);
	assert(result != nullptr);

}


#endif


void GetParameter(lua_State * state, int stack_index, int * result)
{
	assert(state  != nullptr);
	assert(result != nullptr);


	chaos::LuaState L(state);
	L.Push(0);

}



template<typename RET, typename P12>
int LuaFuncCallImpl(lua_State * state)
{

	return 0;
}

template<typename RET, typename P1, typename P2>
int LuaFuncCallImplxxx(lua_State * state)
{
	assert(state != nullptr);

	int upvalue_index = lua_upvalueindex(1); // get the pseudo index for the first upvalue (closure)

	assert(lua_islightuserdata(state, upvalue_index));
	assert(lua_gettop(state) == 2);

	typedef RET (*func_type)(P1, P2);

	func_type func = (func_type)lua_touserdata(state, upvalue_index);

	P1 p1;
	P2 p2;

	GetParameter(state, -2, &p1);
	GetParameter(state, -1, &p2);

	auto result = func(p1, p2);
	chaos::LuaTools::PushValue(state, result);
	return 1;
}

template<typename RET, typename P1, typename P2>
void RegisterGlobalCFunc(lua_State * state, char const * function_name, RET (*func)(P1 p1, P2 p2))
{ 
	assert(state         != nullptr);
	assert(function_name != nullptr);
	assert(func          != nullptr);

	lua_pushlightuserdata(state, func);
	lua_pushcclosure(state, &LuaFuncCallImplxxx<RET, P1, P2>, 1); // 1 for one upvalue (ie. data in closure)
	lua_setglobal(state, function_name);
}


#if 0

template<typename RET, typename P1, typename P2>
void RegisterGlobalCFunc(lua_State * state, char const * function_name, RET (*func)(P1 p1, P2 p2))
{ 
	assert(state         != nullptr);
	assert(function_name != nullptr);
	assert(func          != nullptr);

	lua_pushlightuserdata(state, func);
	lua_pushcclosure(state, &LuaFuncCallImpl<RET, P1, P2>, 1); // 1 for one upvalue (ie. data in closure)
	lua_setglobal(state, function_name);
}

#endif



//BOOST_PP_ENUM_PARAMS(CHAOS_LUABIND_MAXFUNC_PARAMETERS, typename P)

#if 0

// XXX : in lua_pushcclosure, 1 for one upvalue (ie. data in closure)
#define CHAOS_BIND_FUNCTION(z, n, unused)\
  template<typename RET BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, typename P)>\
  void RegisterGlobalCFunc(lua_State * state, char const * function_name, RET (*func)(BOOST_PP_ENUM_BINARY_PARAMS(n,P,p)))\
  {\
    assert(state         != nullptr);\
    assert(function_name != nullptr);\
    assert(func          != nullptr);\
    lua_pushlightuserdata(state, func);\
    lua_pushcclosure(state, &LuaFuncCallImpl<RET BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, P)>, 1);\
    lua_setglobal(state, function_name);\
  }


BOOST_PP_REPEAT(CHAOS_LUABIND_MAXFUNC_PARAMETERS, CHAOS_BIND_FUNCTION, ~)


#endif

/*

*/
// get the pseudo index for the first upvalue (closure)
// XXX : in lua_pushcclosure, 1 for one upvalue (ie. data in closure)
#define CHAOS_BIND_FUNCTION(z, n, unused)\
\
  template<typename RET BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, typename P)>\
  int LuaFuncCallImpl(lua_State * state)\
  {\
    assert(state != nullptr);  \
    int upvalue_index = lua_upvalueindex(1);\
    assert(lua_islightuserdata(state, upvalue_index));\
    assert(lua_gettop(state) == n);\
    typedef RET (*func_type)(BOOST_PP_ENUM_PARAMS(n, P));\
    func_type func = (func_type)lua_touserdata(state, upvalue_index);\
    P1 p1;\
    P2 p2;\
    GetParameter(state, -2, &p1);\
    GetParameter(state, -1, &p2);\
    auto result = func(BOOST_PP_ENUM_PARAMS(n, p));\
    chaos::LuaTools::PushValue(state, result);\
    return 1;\
  }\
\
  template<typename RET BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, typename P)>\
  void RegisterCFunc(lua_State * state, char const * function_name, RET (*func)(BOOST_PP_ENUM_BINARY_PARAMS(n,P,p)))\
  {\
    assert(state         != nullptr);\
    assert(function_name != nullptr);\
    assert(func          != nullptr);\
    lua_pushlightuserdata(state, func);\
    lua_pushcclosure(state, &LuaFuncCallImpl<RET BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, P)>, 1);\
    lua_setglobal(state, function_name);\
  }



BOOST_PP_REPEAT(CHAOS_LUABIND_MAXFUNC_PARAMETERS, CHAOS_BIND_FUNCTION, ~)







#if 0








#endif

int myff(int p1, int p2)
{

	return 888;
}

/*
template<typename RET>
class 
*/


int ff(lua_State * state)
{
	int arg_count = lua_gettop(state);

	lua_Integer p1, p2;

	if (lua_isinteger(state, -1) && lua_isinteger(state, -2))
	{
		p1 = lua_tointeger(state, -1); // parameter 2
		p2 = lua_tointeger(state, -2); // parameter 1

		lua_pushinteger(state, 666); // return value 1
		lua_pushinteger(state, 667); // return value 2
		return 2; // number of result

	}





	return 0;
}


int myff0()
{

	return 888;
}

int myff1(int i1)
{

	return 888;
}

int myff2(int i1, int i2)
{

	return 888;
}

class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		boost::filesystem::path const & rp = GetResourcesPath();

		lua_State * l = chaos::LuaTools::CreateStandardLuaState();
		if (l != nullptr)
		{

			RegisterCFunc(l, "x0", myff0); 
			RegisterCFunc(l, "x1", myff1); 
			RegisterCFunc(l, "x2", myff2); 





			RegisterGlobalCFunc(l, "ff", myff);


			//   lua_pushcfunction(l, ff);
			//   lua_setglobal(l, "ff");


			chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(rp / "test.lua", true);
			if (buffer != nullptr)
			{     
				chaos::LuaTools::ExecBuffer(l, buffer, false);       
			}

			lua_close(l);
		}
		return true;
	}
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);

	return 0;
}


