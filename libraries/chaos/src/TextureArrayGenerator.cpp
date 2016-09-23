#include <chaos/TextureArrayGenerator.h>
#include <chaos/GLTextureTools.h>

namespace chaos
{
  // ========================================================================
  // ImageSliceRegister functions
  // ========================================================================

  bool ImageSliceRegister::InsertSlice(ImageDescription & description, void * user_data)
  {
    // test whether the slice format is valid
    if (!IsImageSliceValid(description))
      return false;      

    // insert the slice into the register
    ImageSliceRegiterEntry entry;
    entry.description = description;
    entry.user_data   = user_data;
    slices.push_back(entry);
    return true;
  }

  bool ImageSliceRegister::IsImageSliceValid(ImageDescription const & description) const
  {
    if (description.bpp != 8 && description.bpp != 24 && description.bpp != 32)
      return false;
    if (description.width <= 0 || description.height <= 0)
      return false;
    if (description.data == nullptr)
      return false;
    return true;
  }

  // ========================================================================
  // ImageLoaderDescriptionGenerator functions
  // ========================================================================

  ImageSliceGeneratorProxy * ImageLoaderSliceGenerator::CreateProxy() const
  {
    FIBITMAP * image = ImageTools::LoadImageFromFile(image_path.string().c_str());
    if (image == nullptr)
      return nullptr;
    return new ImageLoaderSliceGeneratorProxy(image, true);
  }

  // ========================================================================
  // ImageSliceGeneratorProxy functions
  // ========================================================================

  void ImageLoaderSliceGeneratorProxy::AddSlices(ImageSliceRegister & slice_register)
  {
    if (image != nullptr)
      slice_register.InsertSlice(ImageTools::GetImageDescription(image), image);
  }

  void ImageLoaderSliceGeneratorProxy::ReleaseSlices(ImageSliceRegiterEntry * slices, size_t count)
  {
    if (release_image)
    {
      for (size_t i = 0; i < count; ++i)
      {
        FIBITMAP * image = (FIBITMAP *)slices[i].user_data;
        if (image != nullptr)
          continue;
        FreeImage_Unload(image);
      }
    }
  }

  // ========================================================================
  // TextureArrayGenerator functions
  // ========================================================================

  TextureArrayGenerator::TextureArrayGenerator()
  {

  }

  TextureArrayGenerator::~TextureArrayGenerator()
  {
    Clean();
  }

  bool TextureArrayGenerator::AddGenerator(ImageSliceGenerator const & generator, int * slice_index)
  {
    // creates the proxy
    ImageSliceGeneratorProxy * proxy = generator.CreateProxy();
    if (proxy == nullptr)
      return false;
    // insert it into the list
    GeneratorEntry entry;
    entry.proxy       = proxy;
    entry.slice_index = slice_index;
    generators.push_back(entry);
    return true;
  }

  void TextureArrayGenerator::Clean()
  {
    for (auto it : generators)
      delete (it.proxy);
    generators.clear(); // destroy the intrusive_ptr
  }

  boost::intrusive_ptr<Texture> TextureArrayGenerator::GenerateTexture(GenTextureParameters const & parameters) const
  {
    ImageSliceRegister  slice_register;
    std::vector<size_t> slice_counts;
    // insert all slices
    for (size_t i = 0 ; i < generators.size() ; ++i)            
    {
      GeneratorEntry entry = generators[i];

      size_t c1 = slice_register.size();
      entry.proxy->AddSlices(slice_register);
      size_t c2 = slice_register.size();

      slice_counts.push_back(c2 - c1); // insert the count of slices inserted for that generator
    }

    // no slice, no texture
    if (slice_register.size() == 0)
      return nullptr;

    // search max size, max bpp, and final target (GL_TEXTURE_1D_ARRAY or GL_TEXTURE_2D_ARRAY)
    int width  = 0;
    int height = 0;
    int bpp    = 0;
    for (ImageSliceRegiterEntry const & entry : slice_register.slices)
    {
      width  = max(width,  entry.description.width);
      height = max(height, entry.description.height);
      bpp    = max(bpp,    entry.description.bpp);
    }

    // create the texture and fill the slices
    boost::intrusive_ptr<Texture> result = GenerateTexture(slice_register, bpp, width, height, parameters);

    // release slices
    size_t start = 0;
    for (size_t i = 0; i < generators.size(); ++i)
    {
      GeneratorEntry entry = generators[i];

      size_t count = slice_counts[i];
      entry.proxy->ReleaseSlices(&slice_register.slices[start], count); // each proxy is responsible for releasing its own slices
      start += count;
    }

    return result;
  }

  boost::intrusive_ptr<Texture> TextureArrayGenerator::GenerateTexture(ImageSliceRegister & slice_register, int bpp, int width, int height, GenTextureParameters const & parameters) const
  {
    // compute the 'flat' texture target
    GLenum flat_target = GLTextureTools::GetTextureTargetFromSize(width, height, false);
    // convert to 'array' target 
    GLenum array_target = GLTextureTools::ToArrayTextureType(flat_target);

    GenTextureResult result;
    glCreateTextures(array_target, 1, &result.texture_id);
    if (result.texture_id > 0)
    {
      // choose format and internal format (beware FreeImage is BGR/BGRA)
      GLenum internal_format = GLTextureTools::GetTextureFormatsFromBPP(bpp).second;

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      int level_count = GLTextureTools::GetMipmapLevelCount(width, height);
      glTextureStorage3D(result.texture_id, level_count, internal_format, width, height, slice_register.size());

      for (size_t i = 0; i < slice_register.size(); ++i)
      {
        ImageDescription desc = slice_register.slices[i].description;

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 8 * desc.pitch_size / desc.bpp);

        GLenum current_format = GLTextureTools::GetTextureFormatsFromBPP(desc.bpp).first;
        glTextureSubImage3D(result.texture_id, 0, 0, 0, i, desc.width, desc.height, 1, current_format, GL_UNSIGNED_BYTE, desc.data);
      }
            
      result.texture_description.type   = GL_TEXTURE_2D_ARRAY;
      result.texture_description.width  = width;
      result.texture_description.height = height;
      result.texture_description.depth  = slice_register.size();
      result.texture_description.internal_format = internal_format;

      GLTextureTools::GenTextureApplyParameters(result, parameters);

      return new Texture(result.texture_id, result.texture_description);
    }
    return nullptr;
  }

};