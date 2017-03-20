#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/PixelFormat.h>
#include <chaos/ImageDescription.h>

namespace chaos
{
  /**
  * FontTools : provide some static methods to manipulate tools
  */

  class FontTools
  {
  public:

    /** Metrics of a character */
    class CharacterMetrics
    {
    public:

      /** default constructor */
      CharacterMetrics() = default;

      /** constructor with initialization */
      CharacterMetrics(FT_BitmapGlyph in_bitmap_glyph) :
        advance({ in_bitmap_glyph->root.advance.x >> 16, in_bitmap_glyph->root.advance.y >> 16 }),
        width(in_bitmap_glyph->bitmap.width),
        height(in_bitmap_glyph->bitmap.rows),
        bitmap_left(in_bitmap_glyph->left),
        bitmap_top(in_bitmap_glyph->top) {}

      FT_Vector advance{ 0, 0 };
      int       width{ 0 };
      int       height{ 0 };
      int       bitmap_left{ 0 };
      int       bitmap_top{ 0 };
    };

    /** Bitmap + Metrics of a character */
    class CharacterBitmapGlyph : public CharacterMetrics
    {
    public:

      /** default constructor */
      CharacterBitmapGlyph() = default;

      /** constructor with initialization */
      CharacterBitmapGlyph(FT_BitmapGlyph in_bitmap_glyph) :
        CharacterMetrics(in_bitmap_glyph),
        bitmap_glyph(in_bitmap_glyph) {}

      /** the glyph */
      FT_BitmapGlyph bitmap_glyph{ nullptr };
    };

    /** get an image description from a FT_Bitmap object */
    static ImageDescription GetImageDescription(FT_Bitmap & bitmap);

    /** generate a bitmap from a glyph slot */
    static FIBITMAP * GenerateImage(FT_GlyphSlot glyph, PixelFormat const & pixel_format = PixelFormat(PixelFormat::FORMAT_GRAY));
    /** generate a bitmap from a bitmap */
    static FIBITMAP * GenerateImage(FT_Bitmap & bitmap, PixelFormat const & pixel_format = PixelFormat(PixelFormat::FORMAT_GRAY));
    /** generate a bitmap from a font an a string */
    static FIBITMAP * GenerateImage(FT_Face face, char const * str, PixelFormat const & pixel_format = PixelFormat(PixelFormat::FORMAT_GRAY));
    /** generate a bitmap from a font an a character */
    static FIBITMAP * GenerateImage(FT_Face face, char c, PixelFormat const & pixel_format = PixelFormat(PixelFormat::FORMAT_GRAY));
    /** generate a bitmap glyph from a character */
    static FT_BitmapGlyph GetBitmapGlyph(FT_Face face, char c, bool accept_notfound_glyph);
    /** generate a cache with all glyph required for a string */
    static std::map<char, CharacterBitmapGlyph> GetGlyphCacheForString(FT_Face face, char const * str);
  };
};
