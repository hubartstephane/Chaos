#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GraphicResource.h>

namespace chaos
{

  class IndexBuffer : public GraphicResource
  {
  public:

    /** constructor */
    IndexBuffer(GLuint in_id = 0);
    /** destructor */
    virtual ~IndexBuffer();

    /** cleaning the object */
    virtual void Release();

    /** returns the GL name of the resource */
    GLuint GetResourceID() const { return index_buffer_id; }

    /** returns true whether the resource is valid */
    bool IsValid() const { return glIsBuffer(index_buffer_id) == GL_TRUE;}

  protected:

    /** the resource id */
    GLuint index_buffer_id;
  };

}; // namespace chaos
