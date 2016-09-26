#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ImageTools.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/Texture.h>

namespace chaos
{

  /**
  * GenTextureResult : result for gen texture methods
  **/

class GenTextureResult
{
public:

  /** constructor */
  GenTextureResult() : texture_id(0) {}

  /** the id of the texture */
  GLuint             texture_id;
  /** the GL handler for the texture */
  TextureDescription texture_description;
};

/**
 * GenTextureParameters : some parameters to load textures
 **/

class GenTextureParameters
{
public:

  /** constructor */
  GenTextureParameters():
    wrap_s(GL_REPEAT),
    wrap_t(GL_REPEAT),
    wrap_r(GL_REPEAT),
    mag_filter(GL_LINEAR),
    min_filter(GL_LINEAR_MIPMAP_LINEAR),
    level(0),
    border(0),
    build_mipmaps(true),
    rectangle_texture(false){}
  
  /** parameters for glTexParameteri(...) */
  GLenum wrap_s;
  /** parameters for glTexParameteri(...) */
  GLenum wrap_t;
  /** parameters for glTexParameteri(...) */
  GLenum wrap_r;
  /** parameters for glTexParameteri(...) */
  GLenum mag_filter;
  /** parameters for glTexParameteri(...) */
  GLenum min_filter;
  /** parameters for glTexImageXX(...) */
  GLint  level;
  /** parameters for glTexImageXX(...) */
  GLint  border;
  /** enable build mipmaps */
  bool build_mipmaps;
  /** enable the texture to be used has rectangular instead of GL_TEXTURE_1D or GL_TEXTURE_2D */
  bool rectangle_texture;
};


/**
 * GLTextureTools : used to have some generic functions for OpenGL
 **/

class GLTextureTools
{
public:

  /** returns true whether type is an array texture type */
  static bool IsArrayTextureType(GLenum type);
  /** returns true whether type is not an array texture type */
  static bool IsFlatTextureType(GLenum type);

  /** transform a texture type into an array type */
  static GLenum ToArrayTextureType(GLenum type);
  /** transform a texture type into a flat type */
  static GLenum ToFlatTextureType(GLenum type);

  /** Generate a 1D/2D/rectangle texture from an file */
  static GenTextureResult GenTexture(char const * filename, GenTextureParameters const & parameters = GenTextureParameters());
  /** Generate a 1D/2D/rectangle texture from an image */
  static GenTextureResult GenTexture(ImageDescription const & image, GenTextureParameters const & parameters = GenTextureParameters());
  /** Generate a 1D/2D/rectangle texture from an image */
  static GenTextureResult GenTexture(FIBITMAP const * image, GenTextureParameters const & parameters = GenTextureParameters());
  /** Generate a cube texture from a skybox */
  static GenTextureResult GenTexture(SkyBoxImages const * skybox, GenTextureParameters const & parameters = GenTextureParameters());

  /** Generate a texture from lambda */
  static GenTextureResult GenTexture(int width, std::function<void(ImageDescription const &, unsigned char *)> const & func, GenTextureParameters const & parameters = GenTextureParameters());
  static GenTextureResult GenTexture(int width, std::function<void(ImageDescription const &, PixelRGB *)> const & func, GenTextureParameters const & parameters = GenTextureParameters());
  static GenTextureResult GenTexture(int width, std::function<void(ImageDescription const &, PixelRGBA *)> const & func, GenTextureParameters const & parameters = GenTextureParameters());
  static GenTextureResult GenTexture(int width, int height, std::function<void(ImageDescription const &, unsigned char *)> const & func, GenTextureParameters const & parameters = GenTextureParameters());
  static GenTextureResult GenTexture(int width, int height, std::function<void(ImageDescription const &, PixelRGB *)> const & func, GenTextureParameters const & parameters = GenTextureParameters());
  static GenTextureResult GenTexture(int width, int height, std::function<void(ImageDescription const &, PixelRGBA *)> const & func, GenTextureParameters const & parameters = GenTextureParameters());

  /** Generate a 1D/2D/rectangle texture from an file */
  static boost::intrusive_ptr<Texture> GenTextureObject(char const * filename, GenTextureParameters const & parameters = GenTextureParameters());
  /** Generate a 1D/2D/rectangle texture from an image */
  static boost::intrusive_ptr<Texture> GenTextureObject(ImageDescription const & image, GenTextureParameters const & parameters = GenTextureParameters());
  /** Generate a 1D/2D/rectangle texture from an image */
  static boost::intrusive_ptr<Texture> GenTextureObject(FIBITMAP const * image, GenTextureParameters const & parameters = GenTextureParameters());
  /** Generate a cube texture from a skybox */
  static boost::intrusive_ptr<Texture> GenTextureObject(SkyBoxImages const * skybox, GenTextureParameters const & parameters = GenTextureParameters());

  /** Generate a texture from lambda */
  static boost::intrusive_ptr<Texture> GenTextureObject(int width, std::function<void(ImageDescription const &, unsigned char *)> const & func, GenTextureParameters const & parameters = GenTextureParameters());
  static boost::intrusive_ptr<Texture> GenTextureObject(int width, std::function<void(ImageDescription const &, PixelRGB *)> const & func, GenTextureParameters const & parameters = GenTextureParameters());
  static boost::intrusive_ptr<Texture> GenTextureObject(int width, std::function<void(ImageDescription const &, PixelRGBA *)> const & func, GenTextureParameters const & parameters = GenTextureParameters());
  static boost::intrusive_ptr<Texture> GenTextureObject(int width, int height, std::function<void(ImageDescription const &, unsigned char *)> const & func, GenTextureParameters const & parameters = GenTextureParameters());
  static boost::intrusive_ptr<Texture> GenTextureObject(int width, int height, std::function<void(ImageDescription const &, PixelRGB *)> const & func, GenTextureParameters const & parameters = GenTextureParameters());
  static boost::intrusive_ptr<Texture> GenTextureObject(int width, int height, std::function<void(ImageDescription const &, PixelRGBA *)> const & func, GenTextureParameters const & parameters = GenTextureParameters());

  /** returns the maximum number of mipmap */
  static int GetMipmapLevelCount(int width, int height);
  static int GetMipmapLevelCount(int width);
  /** for cubemap texture, returns a layer index depending on the face considered */
  static int GetLayerValueFromCubeMapFace(GLenum face, int level = 0);
  /** Get Format/Internal Format pair from BPP */
  static std::pair<GLenum, GLenum> GetTextureFormatsFromBPP(int bpp);

  /** utility function to compute target (GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE) from dimension */
  static GLenum GetTextureTargetFromSize(int width, int height, bool rectangle_texture);

  /** utility function for texture loading */
  static void GenTextureApplyParameters(GenTextureResult const & result, GenTextureParameters const & parameters);
};

}; // namespace chaos
