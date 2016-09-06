#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
  /**
   * FontTools : provide some static methods to manipulate tools
   */

  class FontTools
  {
  public:

    /** generate a bitmap from a glyph slot */
    static FIBITMAP * GenerateImageFromGlyph(FT_GlyphSlot glyph);
    /** generate a bitmap from a bitmap */
    static FIBITMAP * GenerateImageFromGlyphBitmap(FT_Bitmap & bitmap);
    /** generate a bitmap from a font an a string */
    static FIBITMAP * GenerateImageFromFont(FT_Face face, char const * str);
    /** generate a bitmap glyph from a character */
    static FT_BitmapGlyph GetBitmapGlyph(FT_Face face, char c);  
  };
};
