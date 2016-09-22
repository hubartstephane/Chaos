#include <chaos/TextureArrayGenerator.h>

namespace chaos
{
  TextureArrayGenerator::TextureArrayGenerator()
  {

  }

  TextureArrayGenerator::~TextureArrayGenerator()
  {
    Clean();
  }

  int TextureArrayGenerator::AddGenerator(ImageDescriptionGenerator const & generator, int * slice_index)
  {
    ImageDescriptionGeneratorProxy * proxy = generator.CreateProxy();
    if (proxy == nullptr)
      return -1;
    GeneratorEntry entry;
    entry.proxy       = proxy;
    entry.slice_index = slice_index;
    generators.push_back(entry);
    return (int)(generators.size() - 1);
  }

  void TextureArrayGenerator::Clean()
  {
    for (auto it : generators)
      delete (it.proxy);
    generators.clear(); // destroy the intrusive_ptr
  }

  bool TextureArrayGenerator::IsValid(ImageDescription const & desc) const
  {
    if (desc.bpp != 8 && desc.bpp != 24 && desc.bpp != 32)
      return false;
    if (desc.width <= 0 || desc.height <= 0)
      return false;
    if (desc.data == nullptr)
      return false;
    return true;
  }

  bool TextureArrayGenerator::IsCompatible(ImageDescription const & desc1, ImageDescription const & desc2) const
  {





    return true;
  }

  boost::intrusive_ptr<Texture> TextureArrayGenerator::GenerateTexture(GenTextureParameters const & parameters) const
  {
    Texture * result = nullptr;

    std::vector<ImageDescription> descriptions;
    std::vector<size_t>           description_count;
    for (GeneratorEntry entry : generators)
    {
      size_t c1 = descriptions.size();
      //entry.proxy->
      size_t c2 = descriptions.size();

      description_count.push_back(c2 - c1);
    }

#if 0
    // search max size, max bpp
    int width  = 0;
    int height = 0;
    int bpp    = 0;
    int count  = 0;
    for (auto it : generators)
    {
      ImageDescription desc = it->GetImageDescription();
      if (IsValid(desc))
        break;

      GLenum target = GLTools::GetTextureTargetFromSize(desc.width, desc.height, false);

      width  = max(width,  desc.width);
      height = max(height, desc.height);
      bpp    = max(bpp,    desc.bpp);
      count++;
    }

    GL_TEXTURE_1D_ARRAY

    GLuint texture_id = 0;

    glCreateTextures(target, 1, &texture_id);


//    std::pair<GLenum, GLenum> all_formats = GetTextureFormatsFromBPP(image.bpp);

//    GetTextureTargetFromSize(int width, int height, bool rectangle_texture)


    for (auto it : generators)
      it->ReleaseImageDescription();


#endif
    return result;
  }

};