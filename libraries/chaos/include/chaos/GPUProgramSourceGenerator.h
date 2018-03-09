#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Buffer.h>
#include <chaos/FilePath.h>

namespace chaos
{
  /**
   * GPUProgramSourceGenerator : used to generate GLSL source (or part of source) code from a set of definitions
   */
  
  class GPUProgramSourceGenerator : public ReferencedObject
  {
  public:

    /** destructor */
    virtual ~GPUProgramSourceGenerator() = default;

    /** returns generated code */
    virtual Buffer<char> GenerateSource(std::map<std::string, int> const & definitions)
    { 
      return Buffer<char>();
    }  
  };

  class GPUProgramStringSourceGenerator : public GPUProgramSourceGenerator
  {

  public:

    /** constructor */
    GPUProgramStringSourceGenerator(char const * src);
    /** constructor */
    GPUProgramStringSourceGenerator(Buffer<char> in_buffer);
  
    /** returns generated code */
    virtual Buffer<char> GenerateSource(std::map<std::string, int> const & definitions) override
    {
      return buffer;    
    }

  protected:

    /** this is the cached source code */
    Buffer<char> buffer;
  };

  class GPUProgramFileSourceGenerator : public GPUProgramSourceGenerator
  {
  public:

    /** constructor */
    GPUProgramFileSourceGenerator(FilePath const & path);

    /** returns generated code */
    virtual Buffer<char> GenerateSource(std::map<std::string, int> const & definitions) override
    {
      return buffer;    
    }

  protected:

    /** this is the cached source code */
    Buffer<char> buffer;
  };

}; // namespace chaos
