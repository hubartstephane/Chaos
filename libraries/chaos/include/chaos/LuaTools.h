#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>
#include <chaos/FilePath.h>

namespace chaos
{
  /** utility macro to ensure TOP of lua stack is the same at scope exit */
#define ensure_luatop_const(x)  ensure_luatop_offset(x, 0)
  /** utility macro to ensure TOP of lua stack is the same at scope exit at an offset*/
#define ensure_luatop_offset(x, offset) chaos::LuaTools::ensure_luatop_const_class ensure_luatop_const_##__LINE__(x, offset)

  /** utility macro to force TOP of lua stack to be constant at scope exit */
#define force_luatop_const(x) chaos::LuaTools::force_luatop_const_class force_luatop_const_##__LINE__(x)

/** 
 * LuaTools is namespace-class for methods to handle lua
 */

class LuaTools
{
public:

  /** utility class to ensure TOP of lua stack is the same at scope exit */
  class ensure_luatop_const_class
  {
  public:

    /** constructor */
    ensure_luatop_const_class(lua_State * in_state, int offset = 0) : 
      state(in_state),
      exit_top(lua_gettop(in_state) + offset)
      {
        assert(state != nullptr);
      }
    /** destructor */
    ~ensure_luatop_const_class()
    {
      assert(exit_top == lua_gettop(state));
    }

  protected:

    /** the lua state */
    lua_State * state;
    /** top value */
    int         exit_top;
  };

  /** utility class to force TOP of lua stack to be constant at scope exit */
  class force_luatop_const_class : public ensure_luatop_const_class
  {
  public:

    /** constructor */
    force_luatop_const_class(lua_State * in_state, int in_offset = 0) : ensure_luatop_const_class(in_state, in_offset){}
    /** destructor */
    ~force_luatop_const_class()
    {
      lua_settop(state, exit_top);
    }
  };
  /** type for error handler function */
  typedef int (*ErrorFunction)(lua_State *);
  /** a default function whenever an error happens  */
  static int DefaultErrorFunction(lua_State * state);
  /** a default implementation for allocator function */
  static void * DefaultAllocFunction(void *ud, void *ptr, size_t /* osize */, size_t nsize); 

  /** test whether a given file has a correct lua file extension */ 
  static bool IsLuaFile(FilePathParam const & path);
  /** create a State with default libraries included */
  static lua_State * CreateStandardLuaState(lua_Alloc f = DefaultAllocFunction, void * ud = nullptr);
  /** execute the code the is inside the buffer (it may be compiled or not) . Returns 0 in case of success */           
  static int ExecBuffer(lua_State * state, Buffer<char> const & buffer, bool compiled_buffer = false, char const * chunkname = nullptr, ErrorFunction error_func = DefaultErrorFunction);
  /** execute the code inside a file. It may be compiled or not . Returns 0 in case of success */           
  static int ExecFile(lua_State * state, FilePathParam const & path, bool compiled = false, char const * chunkname = nullptr, ErrorFunction error_func = DefaultErrorFunction);
};

}; // namespace chaos

