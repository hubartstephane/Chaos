#include <chaos/GPUProgramSourceGenerator.h>
#include <chaos/FileTools.h>
#include <chaos/StringTools.h>
#include <chaos/Buffer.h>


namespace chaos
{
  GPUProgramStringSourceGenerator::GPUProgramStringSourceGenerator(char const * src)
  {
    size_t len = strlen(src);
    if (len > 0)
    {
      buffer = SharedBufferPolicy<char>::NewBuffer(len + 1);
      if (buffer != nullptr)
        memcpy(buffer.data, src, len + 1);      
    }
  }
  
  GPUProgramStringSourceGenerator::GPUProgramStringSourceGenerator(Buffer<char> in_buffer) : buffer(in_buffer)
  {

  }

  GPUProgramFileSourceGenerator::GPUProgramFileSourceGenerator(char const * filename)
  {  
    buffer = FileTools::LoadFile(filename, true);
  }

  GPUProgramFileSourceGenerator::GPUProgramFileSourceGenerator(boost::filesystem::path const & path)
  {
    buffer = FileTools::LoadFile(path, true);  
  }

}; // namespace chaos
