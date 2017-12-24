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
    friend class GLProgramData;

  public:

    /** destructor */
    virtual ~RenderMaterial();
    /** cleaning the resource */
    virtual void Release();

    /** prepare the rendering */
    void UseMaterial(GLProgramVariableProvider const * in_uniform_provider) const;

  protected:

    /** the program */
    boost::intrusive_ptr<GLProgram> program;
    /** some rendering states */
    boost::intrusive_ptr<GLProgramVariableProvider> uniform_provider;
    /** parent material */
    boost::intrusive_ptr<RenderMaterial> parent_material;
  };


}; // namespace chaos
