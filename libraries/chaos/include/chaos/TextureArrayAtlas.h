#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/Texture.h>

namespace chaos
{
  namespace BitmapAtlas
  {
    class TextureArrayAtlas : public AtlasBase
    {
    public:

      /** the clearing method */
      virtual void Clear() override;
      /** load an atlas from an index file */
      bool LoadAtlas(boost::filesystem::path const & filename);

    public:

      /** the texture array that will be generated */
      boost::intrusive_ptr<Texture> texture;
    };
  };
};
