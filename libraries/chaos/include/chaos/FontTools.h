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

    /** generate a bitmap from a glyph */
    static FIBITMAP * GenerateImageFromGlyph(FT_GlyphSlot glyph);
    /** generate a bitmap from a font an a string */
    static FIBITMAP * GenerateImageFromFont(FT_Face face, char const * str);
  
  };

};
