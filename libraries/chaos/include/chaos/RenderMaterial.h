#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/ReferencedObject.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUProgramVariableProvider.h>
#include <chaos/GPUFileResource.h>


namespace chaos
{

  /**
   * GPUProgramVariableRenderMaterialProviderChain : this is a variable provider dedicated for RenderMaterials
   */

  class GPUProgramVariableRenderMaterialProviderChain : public GPUProgramVariableProviderChain
  {
  public:

    /** constructor */
    GPUProgramVariableRenderMaterialProviderChain(RenderMaterial const * in_render_material) :
      render_material(in_render_material){}

  protected:

    /** apply the actions */
    virtual bool DoProcessAction(char const * name, GPUProgramVariableAction & action, GPUProgramVariableProvider const * top_provider) const override;

  protected:

    /** the render material as base for the chain */
    RenderMaterial const * render_material = nullptr;
  };

  /**
  * RenderMaterial : this is the combinaison of some uniforms and a program
  */

  class RenderMaterial : public GPUFileResource
  {
    friend class GPUProgramData;
    friend class GPUProgramVariableRenderMaterialProviderChain;

  public:

    /** constructor */
    RenderMaterial();

    /** destructor */
    virtual ~RenderMaterial();
    /** cleaning the resource */
    virtual void Release();

    /** prepare the rendering */
    bool UseMaterial(GPUProgramVariableProvider const * in_uniform_provider) const;

    /** set the program */
    bool SetProgram(GPUProgram * in_program);
    /** set the parent material */
    bool SetParentMaterial(RenderMaterial * in_parent);
    /** go throw the hierary and search for the program */
    GPUProgram const * GetEffectiveProgram() const;

  protected:

    /** the program */
    boost::intrusive_ptr<GPUProgram> program;
    /** some rendering states */
    boost::intrusive_ptr<GPUProgramVariableProvider> uniform_provider;
    /** parent material */
    boost::intrusive_ptr<RenderMaterial> parent_material;
  };


}; // namespace chaos
