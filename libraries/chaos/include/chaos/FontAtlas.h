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
  * FontAtlasGeneratorParams : parameters for generating a font atlas
  */

  using FontAtlasGeneratorParams = TextureAtlasGeneratorParams;

  /**
   * FontAtlasFontParams : parameters for generating a font 
   */

  class FontAtlasFontParams
  {
  public:

    FontAtlasFontParams() :
      glyph_width(32),
      glyph_height(32){}

    /** width of the glyph */
    int glyph_width;
    /** height of the glyph */
    int glyph_height;
  };

  /**
   * FontAtlas : an atlas to hold glyphs
   */

class FontAtlas : public TextureAtlasTypedBase<FontAtlasEntry> {};

  /**
   * FontAtlasInputEntry : an entry for the Font Atlas (this is a sort of big aggregate)
   */

class FontAtlasInputEntry : public TextureAtlasInputEntry
{
public:

  /** constructor */
  FontAtlasInputEntry() :
    library(nullptr),
    face(nullptr),
    release_library(true),
    release_face(true) {}

  /** the characters contained in the entry */
  std::string characters;
  /** the Freetype library if appropriate */
  FT_Library library;
  /** the Freetype face if appropriate */
  FT_Face    face;
  /** should the library be released at destruction */
  bool       release_library;
  /** should the face be released at destruction */
  bool       release_face;
  /** a glyph cache for fonts */
  std::map<char, FontTools::CharacterBitmapGlyph> glyph_cache;
  /** during generation, this vector contains indices of all generated entries (both ENTRIES and both INPUT ENTRIES for standard ATLAS) */
  std::vector<size_t> generated_entries;
  /** the parameters for fonts */
  FontAtlasFontParams font_params;
};

  /**
  * FontAtlasInput : the input to uses to generate a font atlas
  */

class FontAtlasInput : public TextureAtlasInputBase
{

	friend class FontAtlasGenerator;

public:

	/** destructor */
	virtual ~FontAtlasInput(){ Clear(); }
  /** inherited */
  virtual bool AddImageSource(char const * filename, FIBITMAP * image, bool release_bitmap) override;
  /** inherited */
  virtual void Clear() override;
  /** inherited */
  virtual size_t GetEntriesCount() const override { return entries.size(); };

	/** Add a font */
	bool AddFont(FT_Library library, char const * font_name, char const * characters = nullptr, bool release_library = true, FontAtlasFontParams const & font_params = FontAtlasFontParams());
	/** Add a font */
  bool AddFont(FT_Face face, char const * characters = nullptr, bool release_face = true, FontAtlasFontParams const & font_params = FontAtlasFontParams());

protected:

  /** internal method to add a font */
  bool AddFontImpl(FT_Library library, FT_Face face, char const * characters, bool release_library, bool release_face, FontAtlasFontParams const & font_params);

protected:

  /** all the textures for the input */
  std::vector<FontAtlasInputEntry> entries;
};

  /**
  * FontAtlasGenerator : generates a FontAtlas
  */

class FontAtlasGenerator
{
public:

	bool ComputeResult(FontAtlasInput & in_input, FontAtlas & in_output, FontAtlasGeneratorParams const & in_params = FontAtlasGeneratorParams());
};

};
