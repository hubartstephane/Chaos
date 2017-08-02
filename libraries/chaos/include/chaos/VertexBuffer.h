#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GraphicResource.h>

namespace chaos
{

  class VertexBuffer : public GraphicResource
  {
  public:

    /** constructor */
    VertexBuffer(GLuint in_id = 0);
    /** destructor */
    virtual ~VertexBuffer();

    /** cleaning the object */
	virtual void Release() override;

    /** returns the GL name of the resource */
    GLuint GetResourceID() const { return vertex_buffer_id; }

    /** returns true whether the resource is valid */
    bool IsValid() const { return glIsBuffer(vertex_buffer_id) == GL_TRUE;}

  protected:

    /** the resource id */
    GLuint vertex_buffer_id;
  };


}; // namespace chaos
