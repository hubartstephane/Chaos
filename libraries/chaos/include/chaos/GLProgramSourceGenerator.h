#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Buffer.h>


namespace chaos
{
  /**
   * GLProgramSourceGenerator : used to generate GLSL source (or part of source) code from a set of definitions
   */
  
  class GLProgramSourceGenerator : public ReferencedObject
  {
  public:

    /** destructor */
    virtual ~GLProgramSourceGenerator() = default;

    /** returns generated code */
    virtual Buffer<char> GenerateSource(std::map<std::string, int> const & definitions)
    { 
      return Buffer<char>();
    }  
  };

  class GLProgramStringSourceGenerator : public GLProgramSourceGenerator
  {

  public:

    /** constructor */
    GLProgramStringSourceGenerator(char const * src);
    /** constructor */
    GLProgramStringSourceGenerator(Buffer<char> in_buffer);
  
    /** returns generated code */
    virtual Buffer<char> GenerateSource(std::map<std::string, int> const & definitions) override
    {
      return buffer;    
    }

  protected:

    /** this is the cached source code */
    Buffer<char> buffer;
  };

  class GLProgramFileSourceGenerator : public GLProgramSourceGenerator
  {
  public:

    /** constructor */
    GLProgramFileSourceGenerator(char const * filename);
    /** constructor */
    GLProgramFileSourceGenerator(boost::filesystem::path const & path);

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
