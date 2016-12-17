#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLTools.h>
#include <chaos/Texture.h>
#include <chaos/GLMTools.h>

namespace chaos
{
  /**
   * GLProgramUniformAction : base class action to be applyed to Providers
   */

  class GLProgramUniformAction
  {
  public:

    virtual ~GLProgramUniformAction() = default;

    /** processing base scalar types */
    bool Process(char const * name, GLfloat value) { return Process(name, glm::tvec1<GLfloat>(value));}
    bool Process(char const * name, GLdouble value) { return Process(name, glm::tvec1<GLdouble>(value)); }
    bool Process(char const * name, GLboolean value) { return Process(name, glm::tvec1<GLboolean>(value)); }
    bool Process(char const * name, GLint value) { return Process(name, glm::tvec1<GLint>(value)); }
    bool Process(char const * name, GLuint value) { return Process(name, glm::tvec1<GLuint>(value)); }

    /** processing vector types */
    template<typename T> bool Process(char const * name, glm::tvec1<T> const & value)
    {
      return DoProcess(name, GLMTools::RecastVector<glm::tvec4<T>>(value));
    }
    template<typename T> bool Process(char const * name, glm::tvec2<T> const & value)
    {
      return DoProcess(name, GLMTools::RecastVector<glm::tvec4<T>>(value));
    }
    template<typename T> bool Process(char const * name, glm::tvec3<T> const & value)
    {
      return DoProcess(name, GLMTools::RecastVector<glm::tvec4<T>>(value));
    }
    template<typename T> bool Process(char const * name, glm::tvec4<T> const & value)
    {
      return DoProcess(name, GLMTools::RecastVector<glm::tvec4<T>>(value));
    }

    /** processing matrix types */
    bool Process(char const * name, glm::mat2x3 const & value) { return DoProcess(name, glm::mat4(value)); }
    bool Process(char const * name, glm::mat2x4 const & value) { return DoProcess(name, glm::mat4(value)); }
    bool Process(char const * name, glm::mat3x2 const & value) { return DoProcess(name, glm::mat4(value)); }
    bool Process(char const * name, glm::mat3x4 const & value) { return DoProcess(name, glm::mat4(value)); }
    bool Process(char const * name, glm::mat4x2 const & value) { return DoProcess(name, glm::mat4(value)); }
    bool Process(char const * name, glm::mat4x3 const & value) { return DoProcess(name, glm::mat4(value)); }
    bool Process(char const * name, glm::mat2 const & value) { return DoProcess(name, glm::mat4(value)); }
    bool Process(char const * name, glm::mat3 const & value) { return DoProcess(name, glm::mat4(value)); }
    bool Process(char const * name, glm::mat4 const & value) { return DoProcess(name, glm::mat4(value)); }

    bool Process(char const * name, glm::dmat2x3 const & value) { return DoProcess(name, glm::dmat4(value)); }
    bool Process(char const * name, glm::dmat2x4 const & value) { return DoProcess(name, glm::dmat4(value)); }
    bool Process(char const * name, glm::dmat3x2 const & value) { return DoProcess(name, glm::dmat4(value)); }
    bool Process(char const * name, glm::dmat3x4 const & value) { return DoProcess(name, glm::dmat4(value)); }
    bool Process(char const * name, glm::dmat4x2 const & value) { return DoProcess(name, glm::dmat4(value)); }
    bool Process(char const * name, glm::dmat4x3 const & value) { return DoProcess(name, glm::dmat4(value)); }
    bool Process(char const * name, glm::dmat2 const & value) { return DoProcess(name, glm::dmat4(value)); }
    bool Process(char const * name, glm::dmat3 const & value) { return DoProcess(name, glm::dmat4(value)); }
    bool Process(char const * name, glm::dmat4 const & value) { return DoProcess(name, glm::dmat4(value)); }

    /** processing texture */
    bool Process(char const * name, Texture const * value) { return DoProcess(name, value); }

  protected:

    /** the virtual methods to override */
    virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value) { return false; }
    virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value) { return false; }
    virtual bool DoProcess(char const * name, glm::tvec4<GLboolean> const & value) { return false; }
    virtual bool DoProcess(char const * name, glm::tvec4<GLint> const & value) { return false; }
    virtual bool DoProcess(char const * name, glm::tvec4<GLuint> const & value) { return false; }

    virtual bool DoProcess(char const * name, glm::mat4 const & value) { return false; }
    virtual bool DoProcess(char const * name, glm::dmat4 const & value) { return false; }

    virtual bool DoProcess(char const * name, Texture const * value) { return false; }
  };

  /**
   * GLProgramUniformSetUniformAction : action used to initialize an uniform
   */

  class GLProgramUniformSetUniformAction : public GLProgramUniformAction
  {
  public:

    /** constructor */
    GLProgramUniformSetUniformAction(GLUniformInfo const & in_uniform) : uniform(in_uniform) {}

  protected:

    /** the GLProgramUniformAction interface */
    virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value) override { return uniform.SetUniform(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value) override { return uniform.SetUniform(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLboolean> const & value) override { return false; }
    virtual bool DoProcess(char const * name, glm::tvec4<GLint> const & value) override { return uniform.SetUniform(value);}
    virtual bool DoProcess(char const * name, glm::tvec4<GLuint> const & value) override { return uniform.SetUniform(value);}
    virtual bool DoProcess(char const * name, glm::mat4 const & value) override { return uniform.SetUniform(value);}
    virtual bool DoProcess(char const * name, glm::dmat4 const & value) override { return uniform.SetUniform(value);}
    virtual bool DoProcess(char const * name, Texture const * value) override { return uniform.SetUniform(value);}

  protected:

    /** the uniform to set */
    GLUniformInfo const & uniform;
  };

  /**
   * GLProgramUniformGetValueAction : action used to get value for an uniform
   */

  template<typename T>
  class GLProgramUniformGetValueAction : public GLProgramUniformAction
  {
  public:

    /** constructor */
    GLProgramUniformGetValueAction(T & in_result) : result(in_result) {}

  protected:

    virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLboolean> const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLint> const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLuint> const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::mat4 const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::dmat4 const & value) override { return ConvertAndGet(value); }

    virtual bool DoProcess(char const * name, Texture const * value) override
    {

      return false;
    }

  
        
    template<typename U>
    bool ConvertAndGet(U const & value)
    {
      if (DoConvertAndGetVector(value, GLMTools::IsVectorType<T>::type(), GLMTools::IsVectorType<U>::type()))
        return true;
      if (DoConvertAndGetMatrix(value, GLMTools::IsMatrixType<T>::type(), GLMTools::IsMatrixType<U>::type()))
        return true;
      return false;
    }

    /** recasting vector into vector (type and arity) */
    template<typename U, typename B1, typename B2>
    bool DoConvertAndGetVector(U const & value, B1 b1, B2 b2) { return false; }

    template<typename U>
    bool DoConvertAndGetVector(U const & value, boost::mpl::true_, boost::mpl::true_)
    { 
      result = GLMTools::RecastVector<T>(value);
      return true; 
    }

    /** recasting matrix into matrix (type and size) */
    template<typename U, typename B1, typename B2>
    bool DoConvertAndGetMatrix(U const & value, B1 b1, B2 b2) { return false; }

    template<typename U>
    bool DoConvertAndGetMatrix(U const & value, boost::mpl::true_, boost::mpl::true_)
    {
      result = T(value);
      return true;
    }

  protected:

    /** the value we get */
    T & result;
  };

  /**
  * GLProgramUniformProvider : a base class for filling uniforms in a program. The purpose is to take responsability to start an ACTION
  */

  class GLProgramUniformProvider
  {
  public:

    /** destructor */
    virtual ~GLProgramUniformProvider() = default;
    /** the main method : returns try whether tha action has been handled (even if failed) */
    virtual bool ProcessAction(char const * name, GLProgramUniformAction & action) const { return false; }

    /** utility function that deserve to set uniform */
    bool BindUniform(GLUniformInfo const & uniform) const
    {
      return ProcessAction(uniform.name.c_str(), GLProgramUniformSetUniformAction(uniform));
    }

    /** get a value for the uniform */
    template<typename T>
    bool GetValue(char const * name, T & result) const
    {
      return ProcessAction(name, GLProgramUniformGetValueAction<T>(result));
    }
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
      handled_name(in_name), value(in_value) {}

    /** the main method */
    virtual bool ProcessAction(char const * name, GLProgramUniformAction & action) const override
    {
      if (handled_name != name)
        return false;
      return action.Process(name, value);
    }

  protected:

    /** the name of the uniform handled */
    std::string handled_name;
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
      handled_name(in_name), value(in_value) {}
    /** the main method */
    virtual bool ProcessAction(char const * name, GLProgramUniformAction & action) const override;

  protected:

    /** the name of the uniform handled */
    std::string handled_name;
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
    virtual bool ProcessAction(char const * name, GLProgramUniformAction & action) const override;
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
		/** register a uniform texture */
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
