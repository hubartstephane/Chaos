#include <chaos/FilePathManager.h>




namespace chaos
{



FilePathManager::~FilePathManager()
{

}

void FilePathManager::AddPath(char const * dirname, bool recursive)
{
  assert(dirname != nullptr);

}


void FilePathManager::InitializeLuaState(lua_State * state)
{
  assert(state != nullptr);


//  OOLUA::Script sc;
//  sc.register_class<FilePathManager>();

  //OOLUA::register_class<FilePathManager>(state);
  /*  
  OOLUA_PROXY(FilePathManager)
  OOLUA_PROXY_END
  */

  //OOLUA::register_class<FilePathManager>(state);

}

}; // namespace chaos