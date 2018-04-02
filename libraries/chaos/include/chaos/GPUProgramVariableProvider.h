#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUProgramData.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GLTools.h>
#include <chaos/Texture.h>
#include <chaos/GLMTools.h>
#include <chaos/ClassTools.h>

namespace chaos
{
  /**
  * GPUProgramVariableAction : base class action to be applyed to Providers
  */

  class GPUProgramVariableAction
  {
  public:

    /** indicates that the classes in this hierarchy will have a virtual destructor */
    virtual ~GPUProgramVariableAction() = default;

    /** processing base scalar types */
    bool Process(char const * name, GLfloat value) { return Process(name, glm::tvec1<GLfloat>(value)); }
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
  * GPUProgramVariableSetUniformAction : action used to initialize an uniform
  */

  class GPUProgramVariableSetUniformAction : public GPUProgramVariableAction
  {
  public:

    /** constructor */
    GPUProgramVariableSetUniformAction(GLUniformInfo const & in_uniform) : uniform(in_uniform) {}

  protected:

    /** the GPUProgramVariableAction interface */
    virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value) override { return uniform.SetUniform(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value) override { return uniform.SetUniform(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLboolean> const & value) override { return false; }
    virtual bool DoProcess(char const * name, glm::tvec4<GLint> const & value) override { return uniform.SetUniform(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLuint> const & value) override { return uniform.SetUniform(value); }
    virtual bool DoProcess(char const * name, glm::mat4 const & value) override { return uniform.SetUniform(value); }
    virtual bool DoProcess(char const * name, glm::dmat4 const & value) override { return uniform.SetUniform(value); }
    virtual bool DoProcess(char const * name, Texture const * value) override { return uniform.SetUniform(value); }

  protected:

    /** the uniform to set */
    GLUniformInfo const & uniform;
  };

  /**
  * GPUProgramVariableSetUniformAction : action used to initialize an attribute with a default value
  */

  class GPUProgramVariableSetAttributeAction : public GPUProgramVariableAction
  {
  public:

    /** constructor */
    GPUProgramVariableSetAttributeAction(GLAttributeInfo const & in_attribute) : attribute(in_attribute) {}

  protected:

    /** the GPUProgramVariableAction interface */
    virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value) override { return false; }
    virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value) override { return false; }
    virtual bool DoProcess(char const * name, glm::tvec4<GLboolean> const & value) override { return false; }
    virtual bool DoProcess(char const * name, glm::tvec4<GLint> const & value) override { return false; }
    virtual bool DoProcess(char const * name, glm::tvec4<GLuint> const & value) override { return false; }
    virtual bool DoProcess(char const * name, glm::mat4 const & value) override { return false; }
    virtual bool DoProcess(char const * name, glm::dmat4 const & value) override { return false; }
    virtual bool DoProcess(char const * name, Texture const * value) override { return false; }

  protected:

    /** the uniform to set */
    GLAttributeInfo const & attribute;
  };

  /**
  * GPUProgramVariableGetValueAction : action used to get value for an uniform
  */

  template<typename T>
  class GPUProgramVariableGetValueAction : public GPUProgramVariableAction
  {
  public:

    /** constructor */
    GPUProgramVariableGetValueAction(T & in_result) : result(in_result) {}

  protected:

    /** main methods */
    virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLboolean> const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLint> const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLuint> const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::mat4 const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::dmat4 const & value) override { return ConvertAndGet(value); }

    virtual bool DoProcess(char const * name, Texture const * value) override { return false; } // texture not implemented

    /** the generic conversions methods */
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
    bool DoConvertAndGetVector(U const & value, boost::mpl::true_, boost::mpl::true_) // the type expected, and the type of the incomming value are both vector !!
    {
      result = GLMTools::RecastVector<T>(value);
      return true;
    }

    /** recasting matrix into matrix (type and size) */
    template<typename U, typename B1, typename B2>
    bool DoConvertAndGetMatrix(U const & value, B1 b1, B2 b2) { return false; }

    template<typename U>
    bool DoConvertAndGetMatrix(U const & value, boost::mpl::true_, boost::mpl::true_) // the type expected, and the type of the incomming value are both matrix !!
    {
      result = T(value);
      return true;
    }

  protected:

    /** the value we get */
    T & result;
  };

  /**
  * GPUProgramVariableProvider : a base class for filling uniforms or attributes in a program. The purpose is to take responsability to start an ACTION
  */

  class GPUProgramVariableProvider : public ReferencedObject
  {
    friend class GPUProgramVariableProviderChain; // WTF : GPUProgramVariableProviderChain could not call DoProcessAction(...) an another instance without that !!

  public:

    /** the main method : returns try whether tha action has been handled (even if failed) */
    bool ProcessAction(char const * name, GPUProgramVariableAction & action) const
    {
      return DoProcessAction(name, action, this);
    }

    /** utility function that deserve to set uniform */
    bool BindUniform(GLUniformInfo const & uniform) const
    {
      return ProcessAction(uniform.name.c_str(), GPUProgramVariableSetUniformAction(uniform));
    }

    /** utility function that deserve to set attribute */
    bool BindAttribute(GLAttributeInfo const & attribute) const
    {
      return ProcessAction(attribute.name.c_str(), GPUProgramVariableSetAttributeAction(attribute));
    }

    /** get a value for the uniform / attribute */
    template<typename T>
    bool GetValue(char const * name, T & result) const
    {
      return ProcessAction(name, GPUProgramVariableGetValueAction<T>(result));
    }

  protected:

    /** the main method : returns true whether that action has been successfully handled */
    virtual bool DoProcessAction(char const * name, GPUProgramVariableAction & action, GPUProgramVariableProvider const * top_provider) const { return false; }
  };

  /**
  * GPUProgramVariableProviderValue : used to fill GPUProgram binding for uniforms / attribute with simple values
  */

  template<typename T>
  class GPUProgramVariableProviderValue : public GPUProgramVariableProvider
  {
  public:

    /** constructor */
    GPUProgramVariableProviderValue(char const * in_name, T const & in_value) :
      handled_name(in_name), value(in_value) {}

  protected:

    /** the main method */
    virtual bool DoProcessAction(char const * name, GPUProgramVariableAction & action, GPUProgramVariableProvider const * top_provider) const override
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
  * GPUProgramVariableProviderTexture : used to fill GPUProgram binding for a texture
  */

  class GPUProgramVariableProviderTexture : public GPUProgramVariableProvider
  {
  public:

    /** constructor */
    GPUProgramVariableProviderTexture(char const * in_name, boost::intrusive_ptr<Texture> in_value) :
      handled_name(in_name), value(in_value) {}

  protected:

    /** the main method */
    virtual bool DoProcessAction(char const * name, GPUProgramVariableAction & action, GPUProgramVariableProvider const * top_provider) const override;

  protected:

    /** the name of the uniform handled */
    std::string handled_name;
    /** the value of the uniform */
    boost::intrusive_ptr<Texture> value;
  };

  /**
  * GPUProgramVariableProviderChain : used to fill GPUProgram binding for multiple uniforms / uniforms
  */

  class GPUProgramVariableProviderChain : public GPUProgramVariableProvider
  {

  public:

    /** register a uniform value */
    template<typename T>
    void AddVariableValue(char const * name, T const & value)
    {
      AddVariableProvider(new GPUProgramVariableProviderValue<T>(name, value));
    }
    /** register a uniform texture */
    void AddVariableTexture(char const * name, boost::intrusive_ptr<class Texture> texture)
    {
      AddVariableProvider(new GPUProgramVariableProviderTexture(name, texture));
    }
    /** register a generic uniform */
    virtual void AddVariableProvider(GPUProgramVariableProvider * provider);
    /** remove all uniforms for binding */
    virtual void Clear();

  protected:

    /** the main method */
    virtual bool DoProcessAction(char const * name, GPUProgramVariableAction & action, GPUProgramVariableProvider const * top_provider) const override;

  protected:

    /** the uniforms to be set */
    std::vector<boost::intrusive_ptr<GPUProgramVariableProvider>> children_providers;
  };

}; // namespace chaos
