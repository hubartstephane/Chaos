#include <chaos/FontAtlas.h>
#include <chaos/FontTools.h>

namespace chaos
{
  // ========================================================================
  // FontAtlasEntry functions
  // ========================================================================

  void SaveIntoJSON(FontAtlasEntry const & entry, nlohmann::json & json_entry)
  {
    TextureAtlasEntry const & texture_atlas_entry = entry;
    SaveIntoJSON(texture_atlas_entry, json_entry); // call 'super' method

    json_entry["advance_x"]   = entry.advance.x;
    json_entry["advance_y"]   = entry.advance.y;
    json_entry["bitmap_left"] = entry.bitmap_left;
    json_entry["bitmap_top"]  = entry.bitmap_top;
  }

  void LoadFromJSON(FontAtlasEntry & entry, nlohmann::json const & json_entry)
  {
    TextureAtlasEntry & texture_atlas_entry = entry;
    LoadFromJSON(texture_atlas_entry, json_entry); // call 'super' method

    entry.advance.x   = json_entry["advance_x"];
    entry.advance.y   = json_entry["advance_y"];
    entry.bitmap_left = json_entry["bitmap_left"];
    entry.bitmap_top  = json_entry["bitmap_top"];
  }

  // ========================================================================
  // FontAtlasGenerator functions
  // ========================================================================

  bool FontAtlasGenerator::GenerateTextureAtlas(FT_Face face, FontAtlas & atlas, char const * characters, FontTools::GenTextureAtlasParameters const & params)
  {
    assert(face != nullptr);

    if (characters == nullptr)
      characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789<>()[]{}+-*./\\?!;:$@\"'";

    std::map<char, FontTools::CharacterBitmapGlyph> glyph_cache = FontTools::GetGlyphCacheForString(face, characters);

    TextureAtlasInput input;
    char name[] = " ";
    for (auto & glyph : glyph_cache)
    {
      FIBITMAP * bm = FontTools::GenerateImage(glyph.second.bitmap_glyph->bitmap);        
      if (bm != nullptr)
      {        
        name[0] = glyph.first;
        input.AddImageSource(name, bm);   // bitmaps will be released at TextureAtlasData destruction   
      }           
    }      

    // generate the atlas
    TextureAtlas          texture_atlas;
    TextureAtlasGenerator generator;
    bool result = generator.ComputeResult(input, texture_atlas, params.altas_width, params.altas_height, params.padding);

    // release the glyphs
    for (auto glyph_entry : glyph_cache)
      FT_Done_Glyph((FT_Glyph)glyph_entry.second.bitmap_glyph);

    // copy data from texture_atlas to (font_)atlas




    return result;
  }

  bool FontAtlasGenerator::GenerateTextureAtlas(FT_Library library, char const * font_name, FontAtlas & atlas, char const * characters, FontTools::GenTextureAtlasParameters const & params)
  {
    assert(font_name != nullptr);

    bool result         = false;
    bool delete_library = false;

    // create a library if necessary
    if (library == nullptr)
    {
      FT_Error error = FT_Init_FreeType(&library);
      if (error)
        return false;    
      delete_library = true;
    }

    // load the face and set pixel size
    FT_Face face;
    FT_Error error = FT_New_Face(library, font_name, 0, &face);
    if (error == 0)
    {
      error = FT_Set_Pixel_Sizes(face, params.glyph_width, params.glyph_height); // Important else FT_Load_Glyph(...) fails
      if (error == 0)
        result = GenerateTextureAtlas(face, atlas, characters, params);
      FT_Done_Face(face);
    }

    // delete library if necessary
    if (delete_library)
      FT_Done_FreeType(library);

    return result;
  }
};

