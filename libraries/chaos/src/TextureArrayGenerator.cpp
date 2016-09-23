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
    slice_register.push_back(entry);
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
  // TextureArrayGenerator functions
  // ========================================================================

  void ImageLoaderDescriptionGeneratorProxy::AddSlices(ImageSliceRegister & slice_register)
  {
    FIBITMAP * image = ImageTools::LoadImageFromFile(image_path.string().c_str());
    if (image != nullptr)
      slice_register.InsertSlice(ImageTools::GetImageDescription(image), image);
  }

  void ImageLoaderDescriptionGeneratorProxy::ReleaseSlices(ImageSliceRegiterEntry * slices, size_t count)
  {
    for (size_t i = 0 ; i < count ; ++i)
    {
      FIBITMAP * image = (FIBITMAP *)slices[i].user_data;
      if (image != nullptr)
        continue;
      FreeImage_Unload(image);    
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

  int TextureArrayGenerator::AddGenerator(ImageDescriptionGenerator const & generator, int * slice_index)
  {
    // creates the proxy
    ImageDescriptionGeneratorProxy * proxy = generator.CreateProxy();
    if (proxy == nullptr)
      return -1;
    // insert it into the list
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

  boost::intrusive_ptr<Texture> TextureArrayGenerator::GenerateTexture(GenTextureParameters const & parameters) const
  {
    boost::intrusive_ptr<Texture> result;
    
    std::vector<ImageSliceRegiterEntry> slices;
    std::vector<size_t>                 slice_counts;
    // insert all slices
    for (size_t i = 0 ; i < generators.size() ; ++i)            
    {
      GeneratorEntry entry = generators[i];

      size_t c1 = slices.size();
      entry.proxy->AddSlices(ImageSliceRegister(slices));
      size_t c2 = slices.size();

      slice_counts.push_back(c2 - c1); // insert the count of slices inserted for that generator
    }

    // search max size, max bpp, and final target (GL_TEXTURE_1D_ARRAY or GL_TEXTURE_2D_ARRAY)
    int width  = 0;
    int height = 0;
    int bpp    = 0;
    for (ImageSliceRegiterEntry const & entry : slices)
    {
      width  = max(width,  entry.description.width);
      height = max(height, entry.description.height);
      bpp    = max(bpp,    entry.description.bpp);
    }

    // create the texture and fill the slices
    result = GenerateTexture(slices, bpp, width, height, parameters);

    // release slices
    size_t start = 0;
    for (size_t i = 0; i < generators.size(); ++i)
    {
      GeneratorEntry entry = generators[i];

      size_t count = slice_counts[i];
      entry.proxy->ReleaseSlices(&slices[start], count); // each proxy is responsible for releasing its own slices
      start += count;
    }

    return result;
  }

  boost::intrusive_ptr<Texture> TextureArrayGenerator::GenerateTexture(std::vector<ImageSliceRegiterEntry> & slices, int bpp, int width, int height, GenTextureParameters const & parameters) const
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
      glTextureStorage3D(result.texture_id, level_count, internal_format, width, height, slices.size());

      for (size_t i = 0; i < slices.size(); ++i)
      {
        ImageDescription desc = slices[i].description;

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 8 * desc.pitch_size / desc.bpp);

        GLenum current_format = GLTextureTools::GetTextureFormatsFromBPP(desc.bpp).first;
        glTextureSubImage3D(result.texture_id, 0, 0, 0, i, desc.width, desc.height, 1, current_format, GL_UNSIGNED_BYTE, desc.data);
      }
            
      result.texture_description.type   = GL_TEXTURE_2D_ARRAY;
      result.texture_description.width  = width;
      result.texture_description.height = height;
      result.texture_description.depth  = slices.size();
      result.texture_description.internal_format = internal_format;

      GLTextureTools::GenTextureApplyParameters(result, parameters);

      return new Texture(result.texture_id, result.texture_description);
    }
    return nullptr;
  }

};