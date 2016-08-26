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

  int TextureArrayGenerator::AddGenerator(ImageDescriptionGenerator const & generator)
  {
    ImageDescriptionGeneratorProxy * proxy = generator.CreateProxy();
    if (proxy == nullptr)
      return -1;
    generators.push_back(proxy);
    return (int)(generators.size() - 1);
  }

  void TextureArrayGenerator::Clean()
  {
    for (auto it : generators)
      delete (it);
    generators.clear(); // destroy the intrusive_ptr
  }

  bool TextureArrayGenerator::IsValid(ImageDescription const & desc) const
  {
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

    for (auto it : generators)
    {
      ImageDescription Desc = it->GetImageDescription();
      if (IsValid(Desc))
        break;




    }












    return result;
  }

};