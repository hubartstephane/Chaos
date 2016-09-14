#include <chaos/LuaTools.h>
#include <chaos/FileTools.h>
#include <chaos/LogTools.h>


namespace chaos
{
// XXX : whenever a RUNTIME error happens, this handler is called with a string on the stack
//       this is the error message
//       if the source comes from     luaL_loadstring(...)      then the string is prefixed by the source itself (not really readable, not printable)
//       if the source comes from     luaL_loadfile(...)        then the string is prefixed by the filename
//       if the source comes from     luaL_loadbuffer(...)      then ???

int LuaTools::DefaultErrorFunction(lua_State * state)
{
  assert(state != nullptr);
  if (lua_isstring(state, -1))
    LogTools::Log("Lua RUNTIME Error : %s", lua_tostring(state, -1));
  return 0;
}

bool LuaTools::IsLuaFile(char const * filename)
{
  return FileTools::IsTypedFile(filename, "lua");
}

lua_State * LuaTools::CreateStandardLuaState(lua_Alloc f, void *ud)
{
  assert(f != nullptr);

  lua_State * state = lua_newstate (f, ud);
  if (state != nullptr)
    luaL_openlibs(state);
  return state;
}

// if chunkname is a standard string, whenever an error is raised, something like [string "chunkname"] is displayed
// if chunkname begins with @, the error string begins with "chunkname"
int LuaTools::ExecBuffer(lua_State * state, Buffer<char> const & buffer, bool compiled_buffer, char const * chunkname, ErrorFunction error_func)
{
#if _DEBUG
  ensure_luatop_const(state);
#endif

  int nargs    = 0;
  int nresults = 0;
  int errfunc  = 0;

  // push error handler if necessary
  if (error_func != nullptr)
  {
    lua_pushcfunction(state, error_func);
    errfunc = lua_gettop(state);
  }

  char const * error_condition = nullptr;

  int result = luaL_loadbuffer(state, buffer.data, buffer.bufsize - 1, chunkname); // same than luaL_loadstring but with a chunkname. bufsize - 1 for zero terminal
  if (result == 0)
  {
    result = lua_pcall(state, nargs, nresults, errfunc);
    if (result != 0)
      error_condition = "lua_pcall(...)";
  }
  else
    error_condition = "luaL_loadstring(...)";

  // handle errors from luaL_loadstring(...) or lua_pcall(...)
  if (result != 0) 
  {
    // there should be an argument on the stack corresponding to an error (may be a string, may be nil)
    if (lua_isstring(state, -1))
    {
      std::string err = lua_tolstring(state, -1, NULL); 
      LogTools::Log("LuaTools::ExecBuffer error due to %s : %s", error_condition, err.c_str());
    }
    lua_pop(state, 1); // whatever the ErrorHandler do, there will always be one data on the stack (probably a string)
  }

  // if we put an error function on the stack, it should still be top 
  assert(error_func == nullptr || lua_tocfunction(state, errfunc) == error_func); 
  if (error_func != nullptr)
    lua_pop(state, 1);

  return result;
}

int LuaTools::ExecFile(lua_State * state, char const * filename, bool compiled, char const * chunkname, ErrorFunction error_func)
{
  int result = -1;

  // create a state if necessary
  lua_State * new_state = nullptr;
  if (state == nullptr)
  {
    new_state = CreateStandardLuaState();
    if (new_state == nullptr)
      return -1;  // LUA errors are positives
  }
  else
    new_state = state;

  // load file and execute
  Buffer<char> buffer = FileTools::LoadFile(filename, true);
  if (buffer != nullptr)
  {
    if (chunkname == nullptr)
      chunkname = filename;
    result = ExecBuffer(state, buffer, compiled, chunkname, error_func);
  }
  
  // free state if necessary
  if (state == nullptr)
    lua_close(new_state);
  return result;
}

void * LuaTools::DefaultAllocFunction(void *ud, void *ptr, size_t /* osize */, size_t nsize) 
{
  if (nsize == 0) 
  {
    free(ptr);
    return nullptr;
  }
  else
    return realloc(ptr, nsize);
}

}; // namespace chaos
