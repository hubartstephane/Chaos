#include <chaos/Texture.h>


namespace chaos
{

  Texture::Texture(GLuint in_id) : texture_id(in_id)
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
