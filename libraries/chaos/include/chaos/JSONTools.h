#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
  class JSONTools
  {
  public:

    /** parsing a JSON file (catch exceptions) */
    static nlohmann::json Parse(char const * buffer);

    /** reading an attribute (catch exceptions) */
    template<typename T>
    static bool GetAttribute(nlohmann::json entry, char const * name, T & result)
    {
      assert(name != nullptr);
      try
      {
        result = entry.value(name, result);
      }
      catch (...)
      {
        return false;
      }
      return true;
    }
  };

}; // namespace chaos
