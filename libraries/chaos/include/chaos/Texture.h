#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GraphicResource.h>

namespace chaos
{

  class Texture : public GraphicResource
  {
  public:

    /** constructor */
    Texture(GLuint in_id = 0);
    /** destructor */
    virtual ~Texture();

    /** cleaning the object */
    virtual void Release();

    /** returns the GL name of the resource */
    GLuint GetResourceID() const { return texture_id; }

    /** returns true whether the resource is valid */
    bool IsValid() const { return glIsTexture(texture_id) == GL_TRUE;}

  protected:

    /** the resource id */
    GLuint texture_id;
  };

}; // namespace chaos
