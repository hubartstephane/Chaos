#include <chaos/ImageTools.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>


namespace chaos
{

ImageDescription ImageDescription::GetSubImageDescription(int x, int y, int wanted_width, int wanted_height) const
{ 
  ImageDescription result;
  // empty source => empty result
  if (IsEmpty())
    return result;
  // want empty image ?
  if (wanted_width <= 0 || wanted_height <= 0)
    return result;
  // wrong parameters
  if (x < 0 || x + wanted_width > width)
    return result;
  if (y < 0 || y + wanted_height > height)
    return result;

  assert(bpp == 24 || bpp == 32); // do not handle other formats
      
  result.width        = wanted_width;
  result.height       = wanted_height;
  result.bpp          = bpp;
  result.pitch_size   = pitch_size;
  result.line_size    = (result.width * result.bpp) >> 3; 
  result.padding_size = result.pitch_size - result.line_size;

  int offset = 0;    
  offset += (x * result.bpp) >> 3; // horizontal padding
  offset += (y * pitch_size); 

  result.data = ((char*)data) + offset;

  return result;
}

ImageDescription ImageTools::GetImageDescription(FIBITMAP const * image)
{
  assert(image != nullptr);

  ImageDescription result;
  result.width        = FreeImage_GetWidth((FIBITMAP*)image); // FREEIMAGE does use constness correctly
  result.height       = FreeImage_GetHeight((FIBITMAP*)image);
  result.bpp          = FreeImage_GetBPP((FIBITMAP*)image);
  result.line_size    = FreeImage_GetLine((FIBITMAP*)image);
  result.pitch_size   = FreeImage_GetPitch((FIBITMAP*)image);
  result.padding_size = result.pitch_size - result.line_size;
  result.data         = FreeImage_GetBits((FIBITMAP*)image);
  return result;
}

void ImageTools::CopyPixels(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height)
{
  assert(src_desc.bpp == dst_desc.bpp);
  assert(src_desc.bpp == 8 || src_desc.bpp == 24 || src_desc.bpp == 32);
  assert(width  >= 0);
  assert(height >= 0);
  assert(src_x >= 0 && src_x + width  <= src_desc.width);
  assert(src_y >= 0 && src_y + height <= src_desc.height);
  assert(dst_x >= 0 && dst_x + width  <= dst_desc.width);
  assert(dst_y >= 0 && dst_y + height <= dst_desc.height);

  if (src_desc.bpp == 8)
  {
    for (int l = 0 ; l < height ; ++l)
    {
      unsigned char const * s = ImageTools::GetPixelAddress<unsigned char>(src_desc, src_x, src_y + l);
      unsigned char       * d = ImageTools::GetPixelAddress<unsigned char>(dst_desc, dst_x, dst_y + l);
      memcpy(d, s, width * sizeof(unsigned char));
    }
  }
  else if (src_desc.bpp == 24)
  {
    for (int l = 0 ; l < height ; ++l)
    {
      PixelRGB const * s = ImageTools::GetPixelAddress<PixelRGB>(src_desc, src_x, src_y + l);
      PixelRGB       * d = ImageTools::GetPixelAddress<PixelRGB>(dst_desc, dst_x, dst_y + l);
      memcpy(d, s, width * sizeof(PixelRGB));
    }
  }
  else if (src_desc.bpp == 32)
  {
    for (int l = 0 ; l < height ; ++l)
    {
      PixelRGBA const * s = ImageTools::GetPixelAddress<PixelRGBA>(src_desc, src_x, src_y + l);
      PixelRGBA       * d = ImageTools::GetPixelAddress<PixelRGBA>(dst_desc, dst_x, dst_y + l);
      memcpy(d, s, width * sizeof(PixelRGBA));
    }
  }
}

void ImageTools::CopyPixelsWithCentralSymetry(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height)
{
  assert(src_desc.bpp == dst_desc.bpp);
  assert(src_desc.bpp == 8 || src_desc.bpp == 24 || src_desc.bpp == 32);
  assert(width  >= 0);
  assert(height >= 0);
  assert(src_x >= 0 && src_x + width  <= src_desc.width);
  assert(src_y >= 0 && src_y + height <= src_desc.height);
  assert(dst_x >= 0 && dst_x + width  <= dst_desc.width);
  assert(dst_y >= 0 && dst_y + height <= dst_desc.height);

  if (src_desc.bpp == 8)
  {
    for (int l = 0 ; l < height ; ++l)
    {
      unsigned char const * s = ImageTools::GetPixelAddress<unsigned char>(src_desc, src_x, src_y + l);
      unsigned char       * d = ImageTools::GetPixelAddress<unsigned char>(dst_desc, dst_x, dst_y + height - 1 - l);
      for (int c = 0 ; c < width ; ++c)
        d[width - 1 - c] = s[c];
    }
  }
  else if (src_desc.bpp == 24)
  {
    for (int l = 0 ; l < height ; ++l)
    {
      PixelRGB const * s = ImageTools::GetPixelAddress<PixelRGB>(src_desc, src_x, src_y + l);
      PixelRGB       * d = ImageTools::GetPixelAddress<PixelRGB>(dst_desc, dst_x, dst_y + height - 1 - l);
      for (int c = 0 ; c < width ; ++c)
        d[width - 1 - c] = s[c];
    }
  }
  else if (src_desc.bpp == 32)
  {
    for (int l = 0 ; l < height ; ++l)
    {
      PixelRGBA const * s = ImageTools::GetPixelAddress<PixelRGBA>(src_desc, src_x, src_y + l);
      PixelRGBA       * d = ImageTools::GetPixelAddress<PixelRGBA>(dst_desc, dst_x, dst_y + height - 1 - l);
      for (int c = 0 ; c < width ; ++c)
        d[width - 1 - c] = s[c];
    }
  }
}

FIBITMAP * ImageTools::LoadImageFromFile(char const * filename)
{
  assert(filename != nullptr);

  FIBITMAP * result = nullptr;
  
  Buffer<char> buffer = FileTools::LoadFile(filename, false);
  if (buffer != nullptr)
  {
    FIMEMORY * memory = FreeImage_OpenMemory((BYTE*)buffer.data, (DWORD)buffer.bufsize);
    if (memory != nullptr)
    {
      FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(memory, 0);

      result = FreeImage_LoadFromMemory(format, memory, 0);
      if (FreeImage_GetBPP(result) == 8 && FreeImage_GetPalette(result) != nullptr)
      {
        FIBITMAP * other = FreeImage_ConvertTo32Bits(result);
        FreeImage_Unload(result);
        result = other;           // this code is good even if the conversion fails 
      }

      FreeImage_CloseMemory(memory);
    }
  }
  return result;
}

FIMULTIBITMAP * ImageTools::LoadMultiImageFromFile(char const * filename)
{
  assert(filename != nullptr);

  FIMULTIBITMAP * result = nullptr;

  Buffer<char> buffer = FileTools::LoadFile(filename, false);
  if (buffer != nullptr)
  {
    FIMEMORY * memory = FreeImage_OpenMemory((BYTE*)buffer.data, (DWORD)buffer.bufsize);
    if (memory != nullptr)
    {
      FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(memory, 0);

      result = FreeImage_LoadMultiBitmapFromMemory(format, memory, 0);

      FreeImage_CloseMemory(memory);
    }
  }
  return result;
}

}; // namespace chaos
