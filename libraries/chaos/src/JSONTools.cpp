#include <chaos/JSONTools.h>
#include <chaos/FileTools.h>

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

  nlohmann::json JSONTools::LoadJSONFileRecursive(FilePathParam const & path)
  {
    Buffer<char> buffer = FileTools::LoadFile(path, true);
    if (buffer != nullptr)
     return JSONTools::Parse(buffer.data);
    return nlohmann::json();
  }
}; // namespace chaos
