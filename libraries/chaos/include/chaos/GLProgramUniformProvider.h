#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLTools.h>
#include <chaos/Texture.h>

namespace chaos
{

  /**
  * GLProgramUniformProvider : a base class for filling uniforms in a program
  */

  class GLProgramUniformProvider
  {
  public:

    /** destructor */
    virtual ~GLProgramUniformProvider() = default;
    /** the main method */
    virtual bool BindUniform(GLUniformInfo const & uniform) const { return false; }
  };

  /**
  * GLProgramUniformProviderValue : used to fill GLProgram binding for uniforms with simple values
  */

  template<typename T>
  class GLProgramUniformProviderValue : public GLProgramUniformProvider
  {
  public:

    /** constructor */
    GLProgramUniformProviderValue(char const * in_name, T const & in_value) :
      name(in_name), value(in_value) {}
    /** the main method */
    virtual bool BindUniform(GLUniformInfo const & uniform) const 
    { 
      if (uniform.name != name)
        return false; 
      uniform.SetUniform(value);
      return true;
    }

  protected:

    /** the name of the uniform handled */
    std::string name;
    /** the value of the uniform */
    T value;
  };

  /**
  * GLProgramUniformProviderTexture : used to fill GLProgram binding for a texture
  */

  class GLProgramUniformProviderTexture : public GLProgramUniformProvider
  {
  public:

    /** constructor */
    GLProgramUniformProviderTexture(char const * in_name, boost::intrusive_ptr<Texture> in_value) :
      name(in_name), value(in_value) {}
    /** the main method */
    virtual bool BindUniform(GLUniformInfo const & uniform) const;

  protected:

    /** the name of the uniform handled */
    std::string name;
    /** the value of the uniform */
    boost::intrusive_ptr<Texture> value;
  };


	/**
	* GLProgramUniformProviderChain : used to fill GLProgram binding for multiple uniforms
	*/

	class GLProgramUniformProviderChain : public GLProgramUniformProvider
	{

	public:

		/** constructor */
		GLProgramUniformProviderChain(GLProgramUniformProvider * in_next_provider = nullptr):
      next_provider(in_next_provider) {}
		/** the main method */
		virtual bool BindUniform(GLUniformInfo const & uniform) const;
		/** remove all uniforms for binding */
		void Clear()
		{
      children_providers.clear();
		}
		/** register a uniform value */
		template<typename T>
		void AddUniformValue(char const * name, T const & value)
		{
			AddUniform(new GLProgramUniformProviderValue<T>(name, value));
		}
		/** register a texture as uniform */
		void AddUniformTexture(char const * name, boost::intrusive_ptr<class Texture> texture)
		{
      AddUniform(new GLProgramUniformProviderTexture(name, texture));
		}
    /** register a generic uniform */
    void AddUniform(GLProgramUniformProvider * provider)
    {
      children_providers.push_back(std::move(std::unique_ptr<GLProgramUniformProvider>(provider)));
    }

	protected:

		/** responsability chain for providers */
		GLProgramUniformProvider * next_provider;
		/** the uniforms to be set */
		std::vector<std::unique_ptr<GLProgramUniformProvider>> children_providers;
	};

}; // namespace chaos
