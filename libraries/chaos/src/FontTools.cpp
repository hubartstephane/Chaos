#include <chaos/FontTools.h>
#include <chaos/ImageTools.h>


// XXX : for Freetype, a glyph in memory is organised up to down
//   
// ----------------------------------- pen_y
//            |
//            v - bitmap_top
// Line 0       +---+
// Line 1       |   |
// Line 2       +---+
// Line 3       |   |
// Line 4       |   |
// 

namespace chaos
{  
  FIBITMAP * FontTools::GenerateImageFromGlyph(FT_GlyphSlot glyph)
  {
    assert(glyph != nullptr);

    FIBITMAP * result = nullptr;

    FT_Error Err = FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL); // render the slot into a bitmap
    if (!Err)
      result = GenerateImageFromGlyphBitmap(glyph->bitmap); // convert FreeType-bitmap into FreeImage-bitmap
    return result;
  }

  FIBITMAP * FontTools::GenerateImageFromGlyphBitmap(FT_Bitmap & bitmap)
  {
    int w = bitmap.width;
    int h = bitmap.rows;

    FIBITMAP * result = FreeImage_Allocate(w, h, 8);
    if (result != nullptr)
    {
      ImageDescription image_description = ImageTools::GetImageDescription(result);

      unsigned char       * dst = (unsigned char *)image_description.data;
      unsigned char const * src = bitmap.buffer;

      for (int j = 0; j < h; ++j)
        for (int i = 0; i < w; ++i)
          dst[i + j * image_description.pitch_size] = src[i + (h - 1 - j) * w]; // glyph is reversed compare to what we want
    }
    return result;
  }


  FT_BitmapGlyph FontTools::GetBitmapGlyph(FT_Face face, char c, bool accept_notfound_glyph)
  {
    FT_Error error = 0;

    // get glyph index
    int glyph_index = FT_Get_Char_Index(face, c);
    if (glyph_index && !accept_notfound_glyph)
      return nullptr;

    // load the glyph
    error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT); 
    if (error)
      return nullptr;

    FT_Glyph glyph;

    // copy the glyph
    error = FT_Get_Glyph(face->glyph, &glyph); 
    if (error || glyph == nullptr)
      return nullptr;

    // convert to a bitmap if necessary
    if (glyph->format != FT_GLYPH_FORMAT_BITMAP) 
    {
      error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
      if (error)
        return nullptr;
    }

    return (FT_BitmapGlyph)glyph;
  }

  std::map<char, FontTools::CharacterBitmapGlyph> FontTools::GetGlyphCacheForString(FT_Face face, char const * str)
  {
    assert(face != nullptr);
    assert(str  != nullptr); 

    std::map<char, CharacterBitmapGlyph> result;

    for (int i = 0 ; str[i] != 0 ; ++i)
    {
      char c = str[i];

      if (result.find(c) != result.end()) // already in cache
        continue;

      FT_BitmapGlyph bitmap_glyph = GetBitmapGlyph(face, c, true);
      if (bitmap_glyph == nullptr)
        continue;

      result.insert(std::make_pair(c, CharacterBitmapGlyph(bitmap_glyph))); // insert into cache
    }

    return result;
  }

  FIBITMAP * FontTools::GenerateImageFromChar(FT_Face face, char c)
  {
    assert(face != nullptr);

    FIBITMAP * result = nullptr;

    FT_BitmapGlyph bitmap_glyph = GetBitmapGlyph(face, c, true);
    if (bitmap_glyph != nullptr)
    {
      result = GenerateImageFromGlyphBitmap(bitmap_glyph->bitmap);
      FT_Done_Glyph((FT_Glyph)bitmap_glyph);    
    }  
    return result;
  }

  FIBITMAP * FontTools::GenerateImageFromString(FT_Face face, char const * str)
  {
    assert(face != nullptr);
    assert(str  != nullptr);
    
    FIBITMAP * result = nullptr;

    // generate all required glyph
    std::map<char, CharacterBitmapGlyph> glyph_cache = GetGlyphCacheForString(face, str);

    // STEP 1 : compute required size
    int min_x = std::numeric_limits<int>::max();
    int max_x = std::numeric_limits<int>::min();
    int min_y = std::numeric_limits<int>::max();
    int max_y = std::numeric_limits<int>::min();

    int pos_x = 0;
    int pos_y = 0;
    for (int i = 0 ; str[i] != 0 ; ++i)
    {
      char c = str[i];

      auto const it = glyph_cache.find(c);
      if (it == glyph_cache.cend())
        continue;

      CharacterBitmapGlyph const & record = it->second;

      // get the metrics
      int w   = record.width;
      int h   = record.height;
      int bl  = record.bitmap_left;
      int bt  = record.bitmap_top;
      int avx = record.advance.x;
      int avy = record.advance.y;

      bt = -bt;

      // compute the position of the 4 corners
      int x1 = pos_x + bl;
      int x2 = pos_x + bl + w;
      int y1 = pos_y + bt;
      int y2 = pos_y + bt + h;

      // update min/max X,Y
      min_x = min(min_x, min(x1, x2));
      min_y = min(min_y, min(y1, y2));
      max_x = max(max_x, max(x1, x2));
      max_y = max(max_y, max(y1, y2));

      // advance the cursor
      pos_x += avx;
      pos_y += avy;
    }

    // STEP 2 : draw the string
    int required_width  = max_x - min_x;
    int required_height = max_y - min_y;

    result = FreeImage_Allocate(required_width, required_height, 8); // create bitmap
    if (result != nullptr)
    {     
      int pitch = FreeImage_GetPitch(result);

      unsigned char * buffer = FreeImage_GetBits(result);

      memset(buffer, 0, pitch * required_height); // fill with 0

      int pos_x = 0;
      int pos_y = 0;
      for (int i = 0 ; str[i] != 0 ; ++i)
      {
        char c = str[i];

        auto const it = glyph_cache.find(c);
        if (it == glyph_cache.cend())
          continue;

        CharacterBitmapGlyph const & record = it->second;

        // get the metrics
        int w   = record.width;
        int h   = record.height;
        int bl  = record.bitmap_left;
        int bt  = record.bitmap_top;
        int avx = record.advance.x;
        int avy = record.advance.y;

        bt = -bt;

        // copy the glyph to dest buffer : invert lines 
        for (int y = 0 ; y < h ; ++y)
        {
          for (int x = 0 ; x < w ; ++x)
          {
            unsigned char * d = buffer + 
              (pos_x + bl - min_x + x) +
              (required_height - 1 - (pos_y + bt - min_y + y)) * pitch; // compute destination address

            unsigned char const * s = record.bitmap_glyph->bitmap.buffer + x + y * w; // compute source address

            d[0] = max(d[0], s[0]); // 'max' because there can be an overlaps between consecutive characters : want to BLEND
          }
        }
        // advance the cursor
        pos_x += avx;
        pos_y += avy;
      }
    }

    // release the glyphs
    for (auto glyph_entry : glyph_cache)
      FT_Done_Glyph((FT_Glyph)glyph_entry.second.bitmap_glyph);
      
    return result;  
  }
};

