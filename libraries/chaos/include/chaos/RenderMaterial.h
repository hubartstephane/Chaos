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
    void SelectMaterial();

  protected:

    /** the program */
    boost::intrusive_ptr<GLProgram> program;
    /** some rendering states */
    GLProgramVariableProvider variable_provider;
    /** parent material */
    boost::intrusive_ptr<RenderMaterial> parent_material;
  };


}; // namespace chaos
