#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUProgramData.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GLTools.h>
#include <chaos/Texture.h>
#include <chaos/GLMTools.h>
#include <chaos/GPUProgramVariableAction.h>

namespace chaos
{

  /**
  * GPUProgramVariableProvider : a base class for filling uniforms or attributes in a program. The purpose is to take responsability to start an ACTION
  */

  class GPUProgramVariableProvider : public ReferencedObject
  {
    friend class GPUProgramVariableProviderChain; // WTF : GPUProgramVariableProviderChain could not call DoProcessAction(...) an another instance without that !!
    friend class GPUProgramVariableRenderMaterialProviderChain;

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
