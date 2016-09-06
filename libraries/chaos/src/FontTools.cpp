#include <chaos/FontTools.h>

namespace chaos
{
  FIBITMAP * FontTools::GenerateImageFromGlyph(FT_GlyphSlot glyph)
  {
    assert(glyph != nullptr);

    FIBITMAP * result = nullptr;

    FT_Error Err = FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);
    if (!Err)
    {
      int w = (int)glyph->metrics.width  / 64;
      int h = (int)glyph->metrics.height / 64;

      result = FreeImage_Allocate(w, h, 8);
      if (result != nullptr)
      {
        unsigned char       * dst = (unsigned char * )FreeImage_GetBits(result);
        unsigned char const * src = glyph->bitmap.buffer;

        for (int j = 0 ; j < h ; ++j)
          for (int i = 0 ; i < w ; ++i)
            dst[i + j * w] = src[i + (h - 1 - j) * w]; // glyph is reversed compare to what we want
      }       
    }

    return result;
  }

  FIBITMAP * FontTools::GenerateImageFromFont(FT_Face face, char const * str)
  {
    assert(face != nullptr);
    assert(str != nullptr);

    FIBITMAP * result = nullptr;


    return result;  
  }
};

