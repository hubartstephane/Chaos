#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
  /**
   * FilePathManager : a class use to search for a file inside a set of directories
   */

class FilePathManager
{
public:

  /** destructor */
  virtual ~FilePathManager();

  /** add a directory inside the path to search */
  virtual void AddPath(char const * dirname, bool recursive);


  /** Add the functions in lua state */
  static void InitializeLuaState(lua_State * state);


};


}; // namespace chaos
