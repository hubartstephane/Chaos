#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUProgramData.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GLTools.h>
#include <chaos/Texture.h>
#include <chaos/GLMTools.h>

namespace chaos
{
  /**
  * GPUProgramAction : base class action to be applyed to Providers
  */

  class GPUProgramAction
  {
  public:

    /** indicates that the classes in this hierarchy will have a virtual destructor */
    virtual ~GPUProgramAction() = default;

    /** processing base scalar types */
    bool Process(char const * name, GLfloat value) { return Process(name, glm::tvec1<GLfloat>(value)); }
    bool Process(char const * name, GLdouble value) { return Process(name, glm::tvec1<GLdouble>(value)); }
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
    virtual bool DoProcess(char const * name, glm::tvec4<GLint> const & value) { return false; }
    virtual bool DoProcess(char const * name, glm::tvec4<GLuint> const & value) { return false; }

    virtual bool DoProcess(char const * name, glm::mat4 const & value) { return false; }
    virtual bool DoProcess(char const * name, glm::dmat4 const & value) { return false; }

    virtual bool DoProcess(char const * name, Texture const * value) { return false; }
  };

  /**
  * GPUProgramSetUniformAction : action used to initialize an uniform
  */

  class GPUProgramSetUniformAction : public GPUProgramAction
  {
  public:

    /** constructor */
    GPUProgramSetUniformAction(GLUniformInfo const & in_uniform): 
      uniform(in_uniform){}

  protected:

    /** the GPUProgramAction interface */
    virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value) override { return uniform.SetUniform(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value) override { return uniform.SetUniform(value); }
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
  * GPUProgramSetUniformAction : action used to initialize an attribute with a default value
  */

  class GPUProgramSetAttributeAction : public GPUProgramAction
  {
  public:

    /** constructor */
    GPUProgramSetAttributeAction(GLAttributeInfo const & in_attribute): 
      attribute(in_attribute){}

  protected:

    /** the GPUProgramAction interface */
    virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value) override { return false; }
    virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value) override { return false; }
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
  * GPUProgramGetValueAction : action used to get value for an uniform
  */

  template<typename T>
  class GPUProgramGetValueAction : public GPUProgramAction
  {
  public:

    /** constructor */
    GPUProgramGetValueAction(T & in_result) : 
      result(in_result){}

  protected:

    /** main methods */
    virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value) override { return ConvertAndGet(value); }
    virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value) override { return ConvertAndGet(value); }
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

}; // namespace chaos
