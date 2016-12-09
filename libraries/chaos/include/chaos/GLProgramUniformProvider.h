#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLTools.h>
#include <chaos/Texture.h>

namespace chaos
{

  /**
   * GLProgramUniformProvider : used to fill GLProgram binding for uniforms
   */

class GLProgramUniformProvider
{
protected:

  /** UniformProviderBase : hold data to be send as uniform */
  class UniformProviderBase
  {
  public:
    /** destructor */
    virtual ~UniformProviderBase() = default;
    /** set uniform */
    virtual bool BindUniform(GLUniformInfo const & uniform) = 0;
  };

  /** UniformProvider : a typed specialization of UniformProviderBase */
  template<typename T>
  class UniformProvider : public UniformProviderBase
  {
  public:
    /** the constructor */
    UniformProvider(T const & in_value) : value(in_value) {}
    /** setting the uniform */
    virtual bool BindUniform(GLUniformInfo const & uniform) override { return uniform.SetUniform(value); }
    /** the value to be set */
    T value;
  };

public:

  /** constructor */
  GLProgramUniformProvider(GLProgramUniformProvider * in_previous_provider = nullptr):
    previous_provider(in_previous_provider) {}
  /** destructor */
  ~GLProgramUniformProvider() = default;
  /** the main method */
  bool BindUniform(GLUniformInfo const & uniform) const;
  /** remove all uniforms for binding */
  void Clear()
  {
    uniform_map.clear();
  }
  /** register a uniform */
  template<typename T>
  void AddUniform(char const * name, T const & value)
  {
    uniform_map[name] = std::move(        
      std::unique_ptr<UniformProviderBase>(new UniformProvider<T>(value))      
    );
  }

  /** register a texture as uniform */
  void AddTexture(char const * name, boost::intrusive_ptr<class Texture> const & texture)
  {
    texture_map[name] = texture;
  }

protected:

  /** protected implementation of uniform binding */
  virtual bool BindUniformImpl(GLUniformInfo const & uniform) const;

protected:

  /** responsability chain for providers */
  GLProgramUniformProvider * previous_provider;
  /** the uniforms texture to be set */
  std::map<std::string, boost::intrusive_ptr<Texture>> texture_map;
  /** the uniforms to be set */
  std::map<std::string, std::unique_ptr<UniformProviderBase>> uniform_map;
};

}; // namespace chaos
