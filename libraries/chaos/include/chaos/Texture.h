#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResource.h>

namespace chaos
{
  /**
  * TextureDescription : used to describe an openGL texture
  */

  class TextureDescription
  {
  public:

    /** the type of the texture (1D/2D/3D/RECTANGLE/CUBE) */
    GLenum type = GL_NONE;
    /** the format of the texture (RGB / RGBA) */
    GLenum internal_format = GL_NONE;
    /** the image width */
    int    width = 0;
    /** the image height */
    int    height = 0;
    /** the image depth */
    int    depth = 0;
  };

  class Texture : public GPUResource
  {
  public:

    /** constructor */
    Texture(GLuint in_id = 0, TextureDescription const & in_texture_description = TextureDescription(), char const * in_name = nullptr);
    /** destructor */
    virtual ~Texture();

    /** cleaning the object */
	virtual void Release() override;

	/** get the description of the texture */
    TextureDescription const & GetTextureDescription() const { return texture_description;  }
	/** returns true whether the resource is valid */
	bool IsValid() const { return glIsTexture(texture_id) == GL_TRUE;}
	/** returns the GL name of the resource */
	GLuint GetResourceID() const { return texture_id; }
	/** get the name of the texture */
	char const * GetName() const { return name.c_str();}

  protected:

    /** the resource id */
    GLuint texture_id;
    /** the description of the texture */
    TextureDescription texture_description;
	/** the name of the object */
	std::string name;
  };

}; // namespace chaos
