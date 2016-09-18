#include <chaos/FontAtlas.h>

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
};

