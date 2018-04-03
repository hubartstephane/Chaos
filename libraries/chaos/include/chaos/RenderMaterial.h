#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/ReferencedObject.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPUFileResource.h>


namespace chaos
{

  /**
   * GPUProgramRenderMaterialProvider : this is a variable provider dedicated for RenderMaterials
   */

  class GPUProgramRenderMaterialProvider : public GPUProgramProvider
  {
  public:

    /** constructor */
    GPUProgramRenderMaterialProvider(RenderMaterial const * in_render_material, GPUProgramProviderBase const * in_other_provider) :
      render_material(in_render_material),
      other_provider(in_other_provider)
    {}

  protected:

    /** apply the actions */
    virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const override;

  protected:

    /** the render material as base for the chain */
    RenderMaterial const * render_material = nullptr;
    /** another provider (use a non intrusive reference !!!) */
    GPUProgramProviderBase const * other_provider;
  };

  /**
  * RenderMaterial : this is the combinaison of some uniforms and a program
  */

  class RenderMaterial : public GPUFileResource
  {
    friend class GPUProgramData;
    friend class GPUProgramRenderMaterialProvider;

  public:

    /** constructor */
    RenderMaterial();

    /** destructor */
    virtual ~RenderMaterial();
    /** cleaning the resource */
    virtual void Release();

    /** prepare the rendering */
    bool UseMaterial(GPUProgramProviderBase const * in_uniform_provider) const;

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
    boost::intrusive_ptr<GPUProgramProviderBase> uniform_provider;
    /** parent material */
    boost::intrusive_ptr<RenderMaterial> parent_material;
  };


}; // namespace chaos
