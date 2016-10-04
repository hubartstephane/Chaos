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
  // FontAtlasInput functions
  // ========================================================================

void FontAtlasInput::Clear()
{

}

  // ========================================================================
  // FontAtlasGenerator functions
  // ========================================================================

  bool FontAtlasGenerator::ComputeResult(FontAtlasInput & in_input, FontAtlas & in_ouput, FontAtlasGeneratorParams const & in_params)
  {
  
  
	  return true;
  }

  bool FontAtlasGenerator::GenerateTextureAtlas(FT_Face face, FontAtlas & atlas, char const * characters, FontAtlasGeneratorParams const & params) const
  {
    assert(face != nullptr);

    // Important else FT_Load_Glyph(...) fails
    FT_Error error = FT_Set_Pixel_Sizes(face, params.glyph_width, params.glyph_height); 
    if (error != 0)
      return false;

    // select the characters we want in the font
    if (characters == nullptr)
      characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789<>()[]{}+-*./\\?!;:$@\"'";

    std::map<char, FontTools::CharacterBitmapGlyph> glyph_cache = FontTools::GetGlyphCacheForString(face, characters);

    // for each glyph, generate a bitmap
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
    TextureAtlas          texture_atlas; // use a temporary texture_atlas ... will be used to generate a font atlas
    TextureAtlasGenerator generator;

    bool result = generator.ComputeResult(input, texture_atlas, params);

    // copy data from texture_atlas to (font_)atlas
    std::swap(atlas.atlas_images, texture_atlas.atlas_images);

    size_t count = texture_atlas.entries.size();
    atlas.entries.reserve(count);
    for (size_t i = 0 ; i < count ; ++i)
    {
      TextureAtlasEntry const & texture_atlas_entry = texture_atlas.entries[i];

      FontAtlasEntry font_atlas_entry;
      (TextureAtlasEntry &)font_atlas_entry = texture_atlas_entry; 

      font_atlas_entry.advance.x   = 0;
      font_atlas_entry.advance.y   = 0;
      font_atlas_entry.bitmap_left = 0;
      font_atlas_entry.bitmap_top  = 0;

      auto const & it = glyph_cache.find(texture_atlas_entry.filename[0]); // glyph are indexed un a map with char, texture_atlas_entries with a string 
      if (it != glyph_cache.cend())
      {
        font_atlas_entry.advance     = it->second.advance;
        font_atlas_entry.bitmap_left = it->second.bitmap_left;
        font_atlas_entry.bitmap_top  = it->second.bitmap_top;      
      }      
      atlas.entries.push_back(font_atlas_entry);    
    }

    // release the glyphs
    for (auto glyph_entry : glyph_cache)
      FT_Done_Glyph((FT_Glyph)glyph_entry.second.bitmap_glyph);

    return result;
  }

  bool FontAtlasGenerator::GenerateTextureAtlas(FT_Library library, char const * font_name, FontAtlas & atlas, char const * characters, FontAtlasGeneratorParams const & params) const
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
      result = GenerateTextureAtlas(face, atlas, characters, params);
      FT_Done_Face(face);
    }

    // delete library if necessary
    if (delete_library)
      FT_Done_FreeType(library);

    return result;
  }
};

