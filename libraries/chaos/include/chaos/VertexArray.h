#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GraphicResource.h>

namespace chaos
{

  class VertexArray : public GraphicResource
  {
  public:

    /** constructor */
    VertexArray(GLuint in_id = 0);
    /** destructor */
    virtual ~VertexArray();

    /** cleaning the object */
	virtual void Release() override;

    /** returns the GL name of the resource */
    GLuint GetResourceID() const { return vertex_array_id; }

    /** returns true whether the resource is valid */
    bool IsValid() const { return glIsVertexArray(vertex_array_id) == GL_TRUE;}

  protected:

    /** the resource id */
    GLuint vertex_array_id;
  };


}; // namespace chaos
