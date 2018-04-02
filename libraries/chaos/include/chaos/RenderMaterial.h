#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/ReferencedObject.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUProgramVariableProvider.h>
#include <chaos/GPUFileResource.h>


namespace chaos
{

  class RenderMaterial : public GPUFileResource
  {
    friend class GPUProgramData;

  public:

    /** constructor */
    RenderMaterial();

    /** destructor */
    virtual ~RenderMaterial();
    /** cleaning the resource */
    virtual void Release();

    /** prepare the rendering */
    void UseMaterial(GPUProgramVariableProvider const * in_uniform_provider) const;

    /** set the program */
    void SetProgram(GPUProgram * in_program);
    /** set the parent material */
    void SetParentMaterial(RenderMaterial * in_parent);

  protected:

    /** the program */
    boost::intrusive_ptr<GPUProgram> program;
    /** some rendering states */
    boost::intrusive_ptr<GPUProgramVariableProvider> uniform_provider;
    /** parent material */
    boost::intrusive_ptr<RenderMaterial> parent_material;
  };


}; // namespace chaos
