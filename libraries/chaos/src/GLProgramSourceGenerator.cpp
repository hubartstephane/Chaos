#include <chaos/GLProgramSourceGenerator.h>
#include <chaos/FileTools.h>
#include <chaos/StringTools.h>
#include <chaos/Buffer.h>


namespace chaos
{
  GLProgramStringSourceGenerator::GLProgramStringSourceGenerator(char const * src)
  {
    size_t len = strlen(src);
    if (len > 0)
    {
      buffer = SharedBufferPolicy<char>::NewBuffer(len + 1);
      if (buffer != nullptr)
        memcpy(buffer.data, src, len + 1);      
    }
  }
  
  GLProgramStringSourceGenerator::GLProgramStringSourceGenerator(Buffer<char> in_buffer) : buffer(in_buffer)
  {

  }

  GLProgramFileSourceGenerator::GLProgramFileSourceGenerator(char const * filename)
  {  
    buffer = FileTools::LoadFile(filename, true);
  }

  GLProgramFileSourceGenerator::GLProgramFileSourceGenerator(boost::filesystem::path const & path)
  {
    buffer = FileTools::LoadFile(path, true);  
  }

}; // namespace chaos
