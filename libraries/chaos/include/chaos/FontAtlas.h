#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/TextureAtlas.h>
#include <chaos/FontTools.h>

namespace chaos
{

  /**
   * FontAtlasEntry : class to represents an entry inside the font atlas
   */

  class FontAtlasEntry : public TextureAtlasEntry
  {
  public:

    FT_Vector advance;
    int       bitmap_left; // from 'CharacterMetrics' class
    int       bitmap_top;
  };

  void SaveIntoJSON(FontAtlasEntry const & entry, nlohmann::json & json_entry);

  void LoadFromJSON(FontAtlasEntry & entry, nlohmann::json const & json_entry);

  /**
   * FontAtlas : an atlas to hold glyphs
   */

  class FontAtlas : public TextureAtlasTypedBase<FontAtlasEntry> {};

  /**
  * FontAtlasGenerator : generates a FontAtlas
  */

class FontAtlasGenerator
{
public:

  /** generate a font atlas */
  bool GenerateTextureAtlas(FT_Library library, char const * font_name, FontAtlas & atlas, char const * characters = nullptr, FontTools::GenTextureAtlasParameters const & params = FontTools::GenTextureAtlasParameters()) const;
  /** generate a font atlas */
  bool GenerateTextureAtlas(FT_Face face, FontAtlas & atlas, char const * characters = nullptr, FontTools::GenTextureAtlasParameters const & params = FontTools::GenTextureAtlasParameters()) const;
};

};
