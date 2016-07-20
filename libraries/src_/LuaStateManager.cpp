#include <chaos/LuaStateManager.h>

namespace chaos
{

LuaStateManager::LuaStateManager(): encode_buffer(ENCODING_BUFFER_SIZE)
{

}

LuaStateManager::~LuaStateManager()
{
  ClearCaches(); // explicitely because the buffers are Unmanaged !!
}

void LuaStateManager::ClearCaches()
{
  // destroy the caches
  StateCacheMap::iterator it  = state_caches.begin();
  StateCacheMap::iterator end = state_caches.end();
  while (it != end)
  {
    it->second.Release();
    ++it;
  }
  state_caches.clear();        
}

int LuaStateManager::GetStateInheritance(int StateID) const
{
  StateInheritanceMap::const_iterator it = inheritance_map.find(StateID);
  if (it == inheritance_map.cend())
    return STATE_NONE_ID;
  return it->second;
}

lua_State * LuaStateManager::CreateState(int StateID /*= STATE_BASE_ID*/)
{
  lua_State * L = luaL_newstate();
  if (L != NULL)
  {
    if (!RecursiveInitializeState(L, StateID))
    {
      lua_close(L);
      return NULL;
    }
  }
  return L;
}

bool LuaStateManager::DeclareStateInheritance(int StateID, int ParentStateID)
{
  if (ParentStateID == STATE_NONE_ID) // not necessary
    return true;

  if (StateID == STATE_NONE_ID || StateID == ParentStateID) // STATE_NONE_ID has no parent !!
    return false;

  StateInheritanceMap::const_iterator it  = inheritance_map.find(StateID);
  StateInheritanceMap::const_iterator end = inheritance_map.cend();
  if (it != end)
    return (it->second == ParentStateID); // while the entry is already defined, ensure this is coherent
        
  // ensure no circular inheritance
#if _DEBUG
  for (int p = ParentStateID ; p != STATE_NONE_ID ; p = GetStateInheritance(p))
    if (p == StateID)
      return false;
#endif // _DEBUG

  inheritance_map[StateID] = ParentStateID;
  return true;
}

bool LuaStateManager::RestoreStateFromCache(lua_State * L, StateCache const & buffer)
{



  return true;
}


static int WriteLuaDataIntoBuffer(lua_State *L, const void* p, size_t sz, void* ud)
{
  SparseWriteBuffer<> * writer = (SparseWriteBuffer<> *)ud;
  writer->Write(p, sz);
  return 0;
}

bool LuaStateManager::StoreStateIntoCache(lua_State * L, StateCache & buffer)
{
  encode_buffer.Clear(false);

  int error_code = lua_dump(L, WriteLuaDataIntoBuffer, &encode_buffer);

  if (error_code == 0)
  {
    size_t bufsize = encode_buffer.GetWrittenSize();
    
    char * dst = buffer.Allocate(bufsize);
    if (dst != NULL)
    {
      encode_buffer.CopyToBuffer(dst, bufsize);
      return true;
    }
  } 
  return false;
}

bool LuaStateManager::RecursiveInitializeState(lua_State * L, int StateID)
{
  // early exit
  if (StateID == STATE_NONE_ID)
    return true;

  // try to create state from cache
  if (config.bCacheStates)
  {
    StateCacheMap::iterator it = state_caches.find(StateID);
    if (it != state_caches.end())
      return RestoreStateFromCache(L, it->second);
  }

  // initialize the state elsewere
  if (!RecursiveInitializeState(L, GetStateInheritance(StateID))) // initialize dependencies
    return false;
  if (!DoInitializeState(L, StateID))
    return false;

  // store the state in cache
  if (config.bCacheStates)
  {
    StateCache cache;
    if (StoreStateIntoCache(L, cache))
      state_caches[StateID] = cache;
  }
  return true;
}

bool LuaStateManager::DoInitializeBaseState(lua_State * L)
{
  luaL_openlibs(L);
# if 0
  luaopen_base(L);
  luaopen_table(L);
  luaopen_io(L);
  luaopen_string(L);
  luaopen_math(L);
#endif
  return true;
}

bool LuaStateManager::DoInitializeState(lua_State *L, int StateID)
{
  switch(StateID)
  {
  case STATE_BASE_ID:
    return DoInitializeBaseState(L);
  }
  return false; // unknown ID
}

}; // namespace chaos
