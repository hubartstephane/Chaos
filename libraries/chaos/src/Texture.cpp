#include <chaos/Texture.h>


namespace chaos
{

  Texture::Texture(GLuint in_id, TextureDescription const & in_texture_description): 
    texture_id(in_id), 
    texture_description(in_texture_description)
  {
  
  }

  Texture::~Texture()
  {
    Release();
  }

  void Texture::Release()
  {
    if (texture_id != 0)
    {
      glDeleteTextures(1, &texture_id);
      texture_id = 0;    
    }  
  }
}; // namespace chaos
