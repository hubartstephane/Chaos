#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>
#include <chaos/GLProgramSourceGenerator.h>


namespace chaos
{

  /**
   * GLProgramLoaderCacheOptions : this class is used to define how cache sytem can be used
   */

class GLProgramLoaderCacheOptions
{
public:

  /** constructor */
  GLProgramLoaderCacheOptions();

  /** whether we can use the cache as an input */
  bool read_from_cache;
  /** whether we can use the cache as an output */
  bool write_into_cache;

};

  /**
   * GLProgramLoader : this class deserves to generate GPU programs from sources. 
   *                   It is possible to use cache system and to add some definitions so we can generate multiple programs with small macro differences.
   */

class GLProgramLoader
{
public:

  typedef std::vector<GLProgramSourceGenerator *> GeneratorSet;

  typedef std::map<std::string, int> DefinitionSet;

  /** destructor */
  ~GLProgramLoader(){ Reset(); }
  /** reset the content */
  void Reset();
  /** generate a program from the sources */
  GLuint GenerateProgram(DefinitionSet const & definitions = DefinitionSet(), GLProgramLoaderCacheOptions & cache_options = GLProgramLoaderCacheOptions()) const;

  /** add a generator a given shader */
  bool AddSourceGenerator(GLenum shader_type, GLProgramSourceGenerator * generator);

  /** add a source for a given shader */
  bool AddShaderSource(GLenum shader_type, Buffer<char> buffer);
  /** add a source for a given shader */
  bool AddShaderSource(GLenum shader_type, char const * src);
  /** add a source file for a given shader */
  bool AddShaderSourceFile(GLenum shader_type, char const * filename);
  /** add a source file for a given shader */
  bool AddShaderSourceFile(GLenum shader_type, boost::filesystem::path const & path);

  /** generate a string for all definitions */
  static std::string DefinitionsToString(DefinitionSet const & definitions);

protected:

  /** returns true whenever this is a valid shader type */
  static bool IsShaderTypeValid(GLenum shader_type);
  /** generate a shader for a set of sources */
  GLuint GenerateShader(GLenum shader_type, GeneratorSet const & generators, DefinitionSet const & definitions, std::string const & definitions_string) const; 
  /** called just before linkage */
  virtual bool PreLinkProgram(GLuint program) const;

protected:

  /** the shaders */
  std::map<GLenum, GeneratorSet> shaders;
};

}; // namespace chaos
