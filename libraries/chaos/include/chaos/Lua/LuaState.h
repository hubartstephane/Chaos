#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class LuaState;
	
}; // namespace chaos

#else
	
namespace chaos
{
	/** 
	* LuaState is class to handle some function of lua (beware, this does not destroy the state !!)
	*/

	class LuaState
	{
	public:

		/** constructor */
		LuaState() : state(nullptr){}
		/** initialization constructor */
		LuaState(lua_State * in_state) : state(in_state){}
		/** copy constructor */
		LuaState(LuaState const & other) : state(other.state){}

		/** destructor (doesn't destroy the lua state) */
		~LuaState(){}

		/** conversion operator */
		operator lua_State * (){ return state; }
		/** conversion operator */
		operator lua_State const * () const { return state; }


		/** register some functions in the table given at idx (if idx == 0 then use global table) */  
		void OpenLibs(luaL_Reg const * funcs, int idx); // luaL_openlib(...) seems to have become obsolete

														/** debugging function to show the stack */
		void DebugOutputStack(std::ostream & stream);
		/** debugging function to show an element on the stack */
		void DebugOutputStack(std::ostream & stream, int index);

		/** get a global variable (leave the result on the stack) */
		template<typename T>
		void GetGlobal(T var_key)
		{
			int table_index = lua_gettop(state) + 1; // +1 because, the table will be TOP + 1

													 // we want to index global variable with not a string
													 // lua_getglobal(...) => only works with string
													 // we have to do it ourselves
			lua_getglobal(state, "_G");      
			Push(var_key);
			// read the table and remove it from the stack
			lua_gettable(state, table_index);
			lua_remove(state, table_index);
		}
		/** get a global variable (leave the result on the stack) */
		void GetGlobal(char const * var_key)
		{
			assert(var_key != nullptr);
			lua_getglobal(state, var_key);
		}

		/** this read a table / global environment leaving the value top of the stack */
		template<typename T>
		void GetTable(T var_key, int index = 0)
		{   
			if (index == 0)
			{
				GetGlobal(var_key);
			}
			else
			{
				index = lua_absindex(state, index);

				// duplicate the table : (if index is < 0, it would be invalid as soon as stack is changed)
				lua_pushvalue(state, index);
				Push(var_key);    
				lua_gettable(state, -2);

				// the stack now has 2 extra values :
				//   - a copy of the TABLE
				//   - the result
				// destroy the extra copy of the table
				lua_remove(state, -2);
			}
		}

		/** read a string (global or in an array) */
		template<typename T>
		bool Get(T varname, std::string & result, int index = 0)
		{
			force_luatop_const(state);
			GetTable(varname, index);
			if (!lua_isstring(state, -1))
				return false;
			result = lua_tostring(state, -1);
			return true;
		}
		/** read a global number (global or in an array) */
		template<typename T>
		bool Get(T varname, lua_Integer & result, int index = 0)
		{
			force_luatop_const(state);
			GetTable(varname, index);
			if (!lua_isinteger(state, -1))
				return false;
			result = lua_tointeger(state, -1);
			return true;
		}
		/** read a global number (global or in an array) */
		template<typename T>
		bool Get(T varname, lua_Number & result, int index = 0)
		{
			force_luatop_const(state);
			GetTable(varname, index);
			if (!lua_isnumber(state, -1))
				return false;
			result = lua_tonumber(state, -1);
			return true;
		}
		/** read a global boolean (global or in an array) */
		template<typename T>
		bool Get(T varname, bool & result, int index = 0)
		{
			force_luatop_const(state);
			GetTable(varname, index);
			if (lua_isboolean(state, -1))
				return false;
			result = (lua_toboolean(state, -1))? true : false;
			return true;
		}

		/** set a global variable 
		*  value must be on stack (it is consumed by the call)
		*/
		template<typename T>
		void SetGlobal(T var_key)
		{
			int table_index = lua_gettop(state) + 1; // +1 because, the table will be TOP + 1
													 // we want to index global variable with not a string
													 // lua_setglobal(...) => only works with string
													 // we have to do it ourselves
			lua_getglobal(state, "_G");      
			Push(var_key);
			lua_pushvalue(state, -3); // copy the value (the one that will be consumed by lua_settable (...)    
									  // write the table 
			lua_settable(state, table_index);
			// consume the copy of table and the value
			lua_pop(state, 2);
		}

		/** set a global variable 
		*  value must be on stack (it is consumed by the call)
		*/
		void SetGlobal(char const * var_key)
		{
			assert(var_key != nullptr);
			lua_setglobal(state, var_key);
		}

		/** set a variable (global or in table) */
		template<typename K, typename V>
		void Set(K varname, V value, int index = 0)
		{   
			ensure_luatop_const(state);
			if (index == 0)
			{
				Push(value);
				SetGlobal(varname);
			}
			else
			{
				index = lua_absindex(state, index);

				lua_pushvalue(state, index);
				Push(varname);
				Push(value);      
				lua_settable(state, -3);
				lua_pop(state, 1);
			}
		}

		/** generic function to push an value on stack */ 
		void Push(int8_t value);
		/** generic function to push an value on stack */ 
		void Push(int16_t value);
		/** generic function to push an value on stack */ 
		void Push(int32_t value);
		/** generic function to push an value on stack */ 
		void Push(int64_t value);
		/** generic function to push an value on stack */ 
		void Push(uint8_t value);
		/** generic function to push an value on stack */ 
		void Push(uint16_t value);
		/** generic function to push an value on stack */ 
		void Push(uint32_t value);
		/** generic function to push an value on stack */ 
		void Push(uint64_t value);
		/** generic function to push an value on stack */ 
		void Push(bool value);
		/** generic function to push an value on stack */ 
		void Push(float value);
		/** generic function to push an value on stack */ 
		void Push(double value);
		/** generic function to push an value on stack */ 
		void Push(char const * value);
		/** generic function to push an value on stack */ 
		void Push(std::string const & value);

		/** push user data */
		template<typename T>
		void PushUserData(T const & data)
		{
			void * ptr = lua_newuserdata(state, sizeof(T));
			memcpy(ptr, &data, sizeof(T));
		}

		/** get user data with no test */
		template<typename T>
		bool GetUserDataUnsafe(T & result, int index)
		{
			if (!lua_isuserdata(L, index))
				return false;

			void * ptr = lua_touserdata(L, index);
			result = *((T*)ptr);
			return true;
		}

	public:

		/** the state */
		lua_State * state;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
