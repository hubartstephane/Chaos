#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GraphicResource.h>

namespace chaos
{


  /**
  * TextureDescription : used to describe an openGL texture
  */

  class TextureDescription
  {
  public:

    /** constructor */
    TextureDescription() :
      type(GL_NONE),
      internal_format(GL_NONE),
      width(0),
      height(0),
      depth(0),
      texture_id(0) {}

    /** the type of the texture (1D/2D/3D/RECTANGLE/CUBE) */
    GLenum type;
    /** the format of the texture (RGB / RGBA) */
    GLenum internal_format;
    /** the image width */
    int    width;
    /** the image height */
    int    height;
    /** the image depth */
    int    depth;
    /** the GL handler for the texture */
    GLuint texture_id;
  };

  class Texture : public GraphicResource
  {
  public:

    /** constructor */
    Texture(GLuint in_id = 0, TextureDescription const & in_texture_description = TextureDescription());
    /** destructor */
    virtual ~Texture();

    /** cleaning the object */
    virtual void Release();

    /** returns the GL name of the resource */
    GLuint GetResourceID() const { return texture_id; }

    /** returns true whether the resource is valid */
    bool IsValid() const { return glIsTexture(texture_id) == GL_TRUE;}

    TextureDescription const & GetTextureDescription() const { return texture_description;  }

  protected:

    /** the resource id */
    GLuint texture_id;
    /** the description of the texture */
    TextureDescription texture_description;
  };

}; // namespace chaos
