#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{


/** 
 * ImageFaceDescription : data to work with pixels
 */

class ImageDescription
{
public:

  /** constructor */
  ImageDescription() : 
    width(0),
    height(0),
    bpp(0),
    line_size(0),
    pitch_size(0),
    padding_size(0),
    data(nullptr){}

  /** returns true whether the image is empty */
  bool IsEmpty() const { return (width == 0 || height == 0 || data == nullptr); }
  /** get the image information for a sub image */
  ImageDescription GetSubImageDescription(int x, int y, int wanted_width, int wanted_height) const;

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
  /** the buffer */
  void * data;
};


/** 
 * ImageTools : deserve to load some images
 */

class ImageTools
{

  /** class for pixel copy (RGB) */
  typedef struct { char unused[3]; } SIZE3;
  /** class for pixel copy (RGBA) */
  typedef struct { char unused[4]; } SIZE4;

  BOOST_STATIC_ASSERT(sizeof(SIZE3) == 3);
  BOOST_STATIC_ASSERT(sizeof(SIZE4) == 4);

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

