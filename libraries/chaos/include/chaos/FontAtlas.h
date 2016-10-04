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

  class FontAtlasGeneratorParams : public TextureAtlasGeneratorParams
  {
  public:
    
    FontAtlasGeneratorParams() :
      TextureAtlasGeneratorParams(512, 512, 5, 0),
      glyph_width(32),
      glyph_height(32){}

    int glyph_width;
    int glyph_height;
  };

  /**
   * FontAtlas : an atlas to hold glyphs
   */

  class FontAtlas : public TextureAtlasTypedBase<FontAtlasEntry> {};



  /**
  * FontAtlasInput : the input to uses to generate a font atlas
  */

class FontAtlasInput
{

	friend class FontAtlasGenerator;

public:

	/** constructor */
	FontAtlasInput() {}
	/** destructor */
	~FontAtlasInput(){ Clear(); }

	/** clear all the textures */
	void Clear();

	/** Add a font */
	void AddFont(FT_Library library, char const * font_name, char const * characters = nullptr);
	/** Add a font */
	void AddFont(FT_Face face, char const * characters = nullptr);



	FT_Library library;

	FT_Face    face;

};

  /**
  * FontAtlasGenerator : generates a FontAtlas
  */

class FontAtlasGenerator
{
public:

	bool ComputeResult(FontAtlasInput & in_input, FontAtlas & in_ouput, FontAtlasGeneratorParams const & in_params = FontAtlasGeneratorParams());

#if 0

	/** constructor */     
	TextureAtlasGenerator() : input(nullptr), output(nullptr){}
	/** destructor */     
	virtual ~TextureAtlasGenerator(){} 
	/** compute all texture positions */
	


	/** create an atlas from a directory into another directory */
	static bool CreateAtlasFromDirectory(boost::filesystem::path const & src_dir, boost::filesystem::path const & filename, TextureAtlasGeneratorParams const & in_params = TextureAtlasGeneratorParams());

#endif

  /** generate a font atlas */
  bool GenerateTextureAtlas(FT_Library library, char const * font_name, FontAtlas & atlas, char const * characters = nullptr, FontAtlasGeneratorParams const & params = FontAtlasGeneratorParams()) const;
  /** generate a font atlas */
  bool GenerateTextureAtlas(FT_Face face, FontAtlas & atlas, char const * characters = nullptr, FontAtlasGeneratorParams const & params = FontAtlasGeneratorParams()) const;
};

};
