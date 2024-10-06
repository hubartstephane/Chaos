#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	void LuaState::OpenLibs(luaL_Reg const * funcs, int index)
	{
#if _DEBUG
		ensure_luatop_const(state);
#endif

		if (index == 0) // insert in global scope
		{
			int i = 0;
			while (funcs[i].func != nullptr && funcs[i].name != nullptr)
			{
				lua_pushcfunction(state, funcs[i].func);
				lua_setglobal(state, funcs[i].name);
				++i;
			}
		}
		else // insert in a table given by its index
		{
			lua_pushvalue(state, index); // create a duplicate of the table on top of the stack (index becomes invalid, but it is not used anymore)

			int i = 0;
			while (funcs[i].func != nullptr && funcs[i].name != nullptr)
			{
				lua_pushstring(state, funcs[i].name);
				lua_pushcfunction(state, funcs[i].func);  // ... as soon as other arguments are pushed on the stack
				lua_settable(state, -3);                  // ... we can so use this new index that is always valid
				++i;
			}

			lua_pop(state, 1); // remove the copy of the table
		}
	}

	void LuaState::DebugOutputStack(std::ostream & stream, int index)
	{
		stream << index << " : ";

		// display data
		if  (lua_isnil(state, index))
			stream << "NIL";
		else if  (lua_istable(state, index))
			stream << "TABLE";
		else if (lua_isinteger(state, index))
			stream << "INTEGER  [" << lua_tointeger(state, index) << "]";
		else if (lua_isnumber(state, index))
			stream << "NUMBER   [" << lua_tonumber(state, index) << "]";
		else if (lua_isstring(state, index))
			stream << "STRING   [" << lua_tostring(state, index) << "]";
		else if (lua_isboolean(state, index))
			stream << "BOOLEAN  [" << lua_toboolean(state, index) << "]";
		else if (lua_iscfunction(state, index))
			stream << "C FUNCTION ";
		else if (lua_isfunction(state, index))
			stream << "FUNCTION ";
		else if (lua_islightuserdata(state, index))
			stream << "LIGHT USERDATA ";
		else if (lua_isuserdata(state, index))
			stream << "USERDATA ";

		stream << '\n';
	}

	void LuaState::DebugOutputStack(std::ostream & stream)
	{
		int top = lua_gettop(state);
		for (int i = 1 ; i <= top ; ++i)
			DebugOutputStack(stream, i);
	}

	void LuaState::Push(int8_t value)
	{
		lua_pushinteger(state, (lua_Integer)value);
	}

	void LuaState::Push(int16_t value)
	{
		lua_pushinteger(state, (lua_Integer)value);
	}

	void LuaState::Push(int32_t value)
	{
		lua_pushinteger(state, (lua_Integer)value);
	}

	void LuaState::Push(int64_t value)
	{
		lua_pushinteger(state, (lua_Integer)value);
	}

	void LuaState::Push(uint8_t value)
	{
		lua_pushinteger(state, (lua_Integer)value);
	}

	void LuaState::Push(uint16_t value)
	{
		lua_pushinteger(state, (lua_Integer)value);
	}

	void LuaState::Push(uint32_t value)
	{
		lua_pushinteger(state, (lua_Integer)value);
	}

	void LuaState::Push(uint64_t value)
	{
		lua_pushinteger(state, (lua_Integer)value);
	}

	void LuaState::Push(bool value)
	{
		lua_pushboolean(state, value? 1 : 0);
	}

	void LuaState::Push(float value)
	{
		lua_pushnumber(state, (lua_Number)value);
	}

	void LuaState::Push(double value)
	{
		lua_pushnumber(state, (lua_Number)value);
	}

	void LuaState::Push(char const * value)
	{
		assert(value != nullptr);
		lua_pushstring(state, value);
	}

	void LuaState::Push(std::string const & value)
	{
		Push(value.c_str());
	}


}; // namespace chaos
