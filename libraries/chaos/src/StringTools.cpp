#include <chaos/StringTools.h>

namespace chaos
{

  std::string StringTools::Printf(char const * format, ...)
  {
    std::string result;
    va_list va;
    va_start(va, format);
    result = VPrintf(format, va);
    va_end(va);
    return result;
  }

  std::string StringTools::VPrintf(char const * format, va_list va) // well, this function needs to have a smarted behavior for too long output
  {  
    char buffer[4096];
    vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, va); // doesn't count for the zero  
    return buffer;
  }

  std::string StringTools::RemoveEndLine(std::string src)
  {
    size_t len = src.length();
    while (len > 0)
    {
      if (src[len - 1] == '\n')
        src.pop_back();
      else if (src[len - 1] == '\r')
        src.pop_back();
      else 
        break;
      --len;
    }
    return src;
  }

  std::string StringTools::IndentString(size_t count)
  {
    return std::string().insert(0, count, ' ');
  }

}; // namespace chaos
