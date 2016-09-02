#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

  /**
  * PixelRGB : a structure that helps manipulating 24 bit pixel
  **/

  class PixelRGB
  {
  public:
    unsigned char B;
    unsigned char G;
    unsigned char R;
  };

  /**
  * PixelRGBA : a structure that helps manipulating 32 bit pixel
  **/

  class PixelRGBA
  {
  public:
    unsigned char B;
    unsigned char G;
    unsigned char R;
    unsigned char A;
  };


/** 
 * ImageFaceDescription : data to work with pixels
 */

class ImageDescription
{
public:

  /** constructor */
  ImageDescription() : 
    data(nullptr),
    width(0),
    height(0),
    bpp(0),
    line_size(0),
    pitch_size(0),
    padding_size(0){}

  ImageDescription(void * in_data, int in_width, int in_height, int in_bpp, int in_padding = 0):
    data(in_data),
    width(in_width),
    height(in_height),
    bpp(in_bpp),
    padding_size(in_padding)
  {
    assert(width  >= 0);
    assert(height >= 0);
    assert(bpp == 8 || bpp == 24 || bpp == 32);
    assert(padding_size >= 0);

    line_size  = width * bpp / 8;
    pitch_size = line_size + padding_size;  
  }

  /** returns true whether the image is empty */
  bool IsEmpty() const { return (width == 0 || height == 0 || data == nullptr); }
  /** get the image information for a sub image */
  ImageDescription GetSubImageDescription(int x, int y, int wanted_width, int wanted_height) const;

  /** the buffer */
  void * data;
  /** the image width */
  int    width;
  /** the image height */
  int    height;
  /** the bit per pixels : 8, 24 or 32 */
  int    bpp;
  /** size of line in bytes (exclude padding) : width * bpp / 8 */
  int    line_size;
  /** size of line in bytes (including padding) : padding + line_size */
  int    pitch_size;
  /** padding a the end of a line  */
  int    padding_size;

};


/** 
 * ImageTools : deserve to load some images
 */

class ImageTools
{

public:

  /** load an image from file (use our own implementation instead of FreeImage_LoadFromFile to provide our own error management) */
  static FIBITMAP * LoadImageFromFile(char const * filename);
  /** get the image information from a FreeImage */
  static ImageDescription GetImageDescription(FIBITMAP const * image);
  /** copy pixels */
  static void CopyPixels(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height);
  /** copy pixel with symetry */
  static void CopyPixelsWithCentralSymetry(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height);

  /** compute the address of a pixel (type T is usefull to know for pixel size/format) */
  template<typename T> 
  static inline T * GetPixelAddress(ImageDescription & desc, int x, int y)
  {
    return (T *)((char *)desc.data + desc.pitch_size * y) + x;
  }
  /** compute the address of a pixel (type T is usefull to know for pixel size/format) */
  template<typename T> 
  static inline T const * GetPixelAddress(ImageDescription const & desc, int x, int y)
  {
    return (T const *)((char const *)desc.data + desc.pitch_size * y) + x;
  }
};

}; // namespace chaos

