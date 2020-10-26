#include <chaos/Chaos.h>

namespace chaos
{


#if 0

	char const * LuaBinding::TOMT_KEY = "LuaBinding::TOMT";

	char const * LuaBinding::ID_KEY   = "ID";

	void LuaBinding::GetTOMT()
	{
#if _DEBUG
		ensure_luatop_offset(state, +1);
#endif

		// create if necessary the TOMT
		lua_pushstring(state, TOMT_KEY);
		lua_gettable(state, LUA_REGISTRYINDEX);

		if (lua_isnil(state, -1)) // if the entry does not exists, create it
		{
			lua_pop(state, 1);                   // remove previous result (nil)

			lua_newtable(state);                 // create the table
			lua_pushstring(state, TOMT_KEY);     // push the KEY
			lua_pushvalue(state, -2);            // push value (a duplication of the table), this wil be consumed soon
			lua_settable(state, LUA_REGISTRYINDEX);
		}
		assert(lua_istable(state, -1)); // well if there is an object named "LuaBinding::TOMT" in registry that is not a table, there is a problem !
	}

	void LuaBinding::CreateEmptyMT(InheritanceIntrospection::ClassRuntimeID ID, int tomt_index) // leave the MT on the stack
	{
#if _DEBUG
		ensure_luatop_offset(state, +1);
#endif

		if (tomt_index != 0)
			tomt_index = lua_absindex(state, tomt_index);

		lua_newtable(state);                      // create the MT

												  // register the MT into TOMT
		lua_pushlightuserdata(state, (void*)ID);  // KEY
		lua_pushvalue(state, -2);                 // VALUE = duplicate the MT table
		lua_settable(state, tomt_index);          // consume both KEY and DUPLICATED VALUE

												  // link MT and RuntimeClassID  (MT["ID"] = ID)
		lua_pushstring(state, ID_KEY);            // KEY 
		lua_pushlightuserdata(state, (void*)ID);  // VALUE
		lua_settable(state, -3); 
	}

	void LuaBinding::GetMT(InheritanceIntrospection::ClassRuntimeID ID) // leave the MT on the stack
	{
#if _DEBUG
		ensure_luatop_offset(state, +1);
#endif

		GetTOMT();
		lua_pushlightuserdata(state, (void *)ID);
		lua_gettable(state, -2); // read the MT from TOMT

		lua_remove(state, -2); // remove the TOMT from the stack
	}

	InheritanceIntrospection::ClassRuntimeID LuaBinding::GetClassRuntimeIDFromMT(int mt_index)
	{
#if _DEBUG
		ensure_luatop_const(state);
#endif

		InheritanceIntrospection::ClassRuntimeID result = nullptr;

		if (mt_index != 0)
			mt_index = lua_absindex(state, mt_index);

		lua_pushstring(state, ID_KEY);
		lua_gettable(state, mt_index);

		if (lua_islightuserdata(state, -1))
			result = (InheritanceIntrospection::ClassRuntimeID)lua_touserdata(state, -1);

		lua_pop(state, 1); // remove the lightuserdata

		return result;
	}

	InheritanceIntrospection::ClassRuntimeID LuaBinding::GetClassRuntimeID(int index)
	{
#if _DEBUG
		ensure_luatop_const(state);
#endif

		InheritanceIntrospection::ClassRuntimeID result = nullptr;

		if (lua_isuserdata(state, index))
		{
			if (index != 0)
				index = lua_absindex(state, index);

			lua_getmetatable(state, index);

			result = GetClassRuntimeIDFromMT(-1);

			lua_pop(state, 1); // remove the metatable
		}
		return result;  
	};


#endif

}; // namespace chaos
