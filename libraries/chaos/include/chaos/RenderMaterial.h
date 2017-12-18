#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/ReferencedObject.h>
#include <chaos/GLProgram.h>
#include <chaos/GLProgramVariableProvider.h>
#include <chaos/GraphicResource.h>


namespace chaos
{

  class RenderMaterial : public GraphicResource
  {
  public:

    /** destructor */
    virtual ~RenderMaterial();
    /** cleaning the resource */
    virtual void Release();

    /** prepare the rendering */
    void UseMaterial(GLProgramVariableProvider * uniform_provider);

  protected:

    /** the program */
    boost::intrusive_ptr<GLProgram> program;
    /** some rendering states */
    boost::intrusive_ptr<GLProgramVariableProvider> variable_provider;
    /** parent material */
    boost::intrusive_ptr<RenderMaterial> parent_material;
  };


}; // namespace chaos
