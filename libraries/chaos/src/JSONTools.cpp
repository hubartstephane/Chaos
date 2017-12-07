#include <chaos/JSONTools.h>

namespace chaos
{
 
  nlohmann::json JSONTools::Parse(char const * buffer)
  {
    assert(buffer != nullptr);
    try
    {
      return nlohmann::json::parse(buffer);
    } 
    catch (...)
    {

    }
    return nlohmann::json();
  }

}; // namespace chaos
