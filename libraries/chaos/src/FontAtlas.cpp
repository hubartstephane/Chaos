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

  bool FontAtlasInput::AddImageSource(char const * name, FIBITMAP * image, bool release_bitmap)
  {
    assert(name  != nullptr);
    assert(image != nullptr);

    FontAtlasInputEntry new_entry;

    new_entry.bitmap          = image;
    new_entry.width           = (int)FreeImage_GetWidth(new_entry.bitmap);
    new_entry.height          = (int)FreeImage_GetHeight(new_entry.bitmap);
    new_entry.bpp             = (int)FreeImage_GetBPP(new_entry.bitmap);
    new_entry.name            = name;
    new_entry.release_bitmap  = release_bitmap;

    entries.push_back(std::move(new_entry)); // move for std::string copy

    return true;
  }

  void FontAtlasInput::Clear()
  {
    // destroy the entries
    for (FontAtlasInputEntry & input_entry : entries)
    {
      // release bitmap
      if (input_entry.bitmap != nullptr)
        if (input_entry.release_bitmap)
          FreeImage_Unload(input_entry.bitmap);
      // release face
      if (input_entry.face != nullptr)
        if (input_entry.release_face)
          FT_Done_Face(input_entry.face);
      // release library
      if (input_entry.library != nullptr)
        if (input_entry.release_library)
          FT_Done_FreeType(input_entry.library);
    }
    entries.empty();
  }

  bool FontAtlasInput::AddFont(FT_Library library, char const * font_name, char const * characters, bool release_library, FontAtlasFontParams const & font_params)
  {
    assert(font_name != nullptr);

    // create a library if necessary
    if (library == nullptr)
    {
      FT_Error error = FT_Init_FreeType(&library);
      if (error)
        return false;
      release_library = true;
    }

    // load the face and set pixel size
    FT_Face face;
    FT_Error error = FT_New_Face(library, font_name, 0, &face);
    if (error)
    {      
      if (release_library)
        FT_Done_FreeType(library); // delete library if necessary
      return false;
    }

    return AddFontImpl(library, face, characters, release_library, true, font_params);
  }

  bool FontAtlasInput::AddFont(FT_Face face, char const * characters, bool release_face, FontAtlasFontParams const & font_params)
  {
    return AddFontImpl(nullptr, face, characters, false, release_face, font_params);
  }

  bool FontAtlasInput::AddFontImpl(FT_Library library, FT_Face face, char const * characters, bool release_library, bool release_face, FontAtlasFontParams const & font_params)
  {
    assert(face != nullptr);

    FontAtlasInputEntry new_entry;
    new_entry.library         = library;
    new_entry.face            = face;
    new_entry.release_library = release_library;
    new_entry.release_face    = release_face;
    new_entry.font_params     = font_params;
    if (characters != nullptr)
      new_entry.characters = characters;    
    entries.push_back(std::move(new_entry)); // move for std::string copy

    return true;
  }

  // ========================================================================
  // FontAtlasGenerator functions
  // ========================================================================

  bool FontAtlasGenerator::ComputeResult(FontAtlasInput & in_input, FontAtlas & in_output, FontAtlasGeneratorParams const & in_params)
  {
    // description : -we use a STANDARD FONT and STANDARD FONT GENERATOR to do the job
    //               -then we steal the results into the FONT ATLAS
    //               -part of the job is to be able to fullfill missing members from 'TextureAtlasEntry' and 'FontAtlasEntry'

    // if user does not provide a list of charset for the fonts, use this hard coded one
    static char const * DEFAULT_CHARACTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789<>()[]{}+-*./\\?!;:$@\"'";

    // fill standard atlas input from entries
    TextureAtlasInput std_atlas_input;
    for (FontAtlasInputEntry & entry : in_input.entries)
    {
      // standard bitmap : conversion is direct
      if (entry.bitmap != nullptr)
      {
        std_atlas_input.AddImageSource(entry.name.c_str(), entry.bitmap, false); // standard texture generator is not responsible for BITMAP destruction. The FontAtlasInputEntry is responsible of that
        continue;
      }

      // font entry : one FONT INPUT ENTRY may generate multiple STANDARD INPUT ENTRIES
      if (entry.face != nullptr)
      {       
        // set font size
        FT_Error error = FT_Set_Pixel_Sizes(entry.face, entry.font_params.glyph_width, entry.font_params.glyph_height);
        if (error != 0)
          continue;

        // generate glyph cache 
        char const * characters = (entry.characters.length() != 0) ? entry.characters.c_str() : DEFAULT_CHARACTERS;

        entry.glyph_cache = FontTools::GetGlyphCacheForString(entry.face, characters);

        // transform glyph cache into bitmaps for the texture atlas generator
        char name[] = " ";
        for (auto & glyph : entry.glyph_cache)
        {
          FIBITMAP * bm = FontTools::GenerateImage(glyph.second.bitmap_glyph->bitmap);
          if (bm != nullptr)
          {
            name[0] = glyph.first;            
            if (std_atlas_input.AddImageSource(name, bm, true))   // bitmaps will be released at TextureAtlasData destruction   
              entry.generated_entries.push_back(std_atlas_input.GetEntriesCount() - 1);
          }
        }
        continue;
      }
    }

    // generate the atlas
    TextureAtlas          std_atlas; // use a temporary texture_atlas ... will be used to generate a font atlas
    TextureAtlasGenerator generator;

    bool result = generator.ComputeResult(std_atlas_input, std_atlas, in_params);
    if (result)
      TransformStandardAtlasIntoFontAtlas(std_atlas, in_output, in_input);
 
    // release glyph caches : glyph_cache is only a member for WORK so, 
    //                        it is important to do it here (and not input destruction) because user may call this function twice without changing the INPUT !
    //                        this would cause memory leak
    for (FontAtlasInputEntry & entry : in_input.entries)
    {
      for (auto & glyph : entry.glyph_cache)
        FT_Done_Glyph((FT_Glyph)glyph.second.bitmap_glyph);
      entry.glyph_cache.clear();
      entry.generated_entries.clear();
    }

	  return result;
  }

  void FontAtlasGenerator::TransformStandardAtlasIntoFontAtlas(TextureAtlas & std_atlas, FontAtlas & font_atlas, FontAtlasInput & font_atlas_input)
  {
    // steal data from texture_atlas to (font_)atlas
    std::swap(font_atlas.atlas_images, std_atlas.atlas_images);

    // convert standard atlas entries into font atlas entries (missing information to be fullfilled later)
    size_t count = std_atlas.entries.size();
    font_atlas.entries.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
      TextureAtlasEntry const & std_atlas_entry = std_atlas.entries[i];

      FontAtlasEntry font_atlas_entry;
      (TextureAtlasEntry &)font_atlas_entry = std_atlas_entry;

      font_atlas_entry.advance.x = 0;
      font_atlas_entry.advance.y = 0;
      font_atlas_entry.bitmap_left = 0;
      font_atlas_entry.bitmap_top = 0;

      font_atlas.entries.push_back(font_atlas_entry);
    }

    // XXX : for standard atlas, there is a ONE for ONE correspondance between INPUT_ENTRIES and ENTRIES
    //
    //       for font atlas, this is not the case. some entries corresponds to a whole FONT with multiple bitmaps
    //       'generated_entries' keep a correspondance between
    //
    //          1 FONT INPUT ENTRY => all FONT ENTRIES generated
    //
    //       we use an intermediate standard atlas for the computation. 
    //       the trick is that there is finally a ONE for ONE correspondance between
    //
    //          - STANDARD ATLAS ENTRY       and FONT ATLAS ENTRY
    //          - STANDARD ATLAS INPUT ENTRY and FONT ATLAS ENTRY

    for (size_t i = 0; i < font_atlas_input.entries.size(); ++i) // fill missing information like ADVANCE, BITMAP_LEFT, and BITMAP_RIGHT
    {
      FontAtlasInputEntry & input_entry = font_atlas_input.entries[i];
      for (size_t j = 0; j < input_entry.generated_entries.size(); ++j)
      {
        FontAtlasEntry & entry = font_atlas.entries[input_entry.generated_entries[j]];

        auto const & it = input_entry.glyph_cache.find(entry.name[0]); // glyph are indexed un a map with char, texture_atlas_entries with a string 
        if (it != input_entry.glyph_cache.cend())
        {
          entry.advance = it->second.advance;
          entry.bitmap_left = it->second.bitmap_left;
          entry.bitmap_top = it->second.bitmap_top;
        }
      }
    }

  }
};

