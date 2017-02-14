#include <chaos/ImageTools.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>


namespace chaos
{
	FIBITMAP * ImageTools::GenFreeImage(PixelFormat const & pixel_format, int width, int height)
	{
		FIBITMAP * result = nullptr;

		// get corresponding pixel format
		if (!pixel_format.IsValid())
			return result;

		// get freeimage format
		int bpp = 0;
		FREE_IMAGE_TYPE image_type = GetFreeImageType(pixel_format, &bpp);
		if (image_type == FIT_UNKNOWN)
			return nullptr;

		// allocate the freeimage
		return FreeImage_Allocate(image_type, width, height, bpp);
	}


	FREE_IMAGE_TYPE ImageTools::GetFreeImageType(PixelFormat const & pixel_format, int * bpp)
	{
		if (pixel_format.component_type == PixelFormat::TYPE_UNSIGNED_CHAR)
		{
			if (pixel_format.component_count == 1 || pixel_format.component_count == 3 || pixel_format.component_count == 4)
			{
				if (bpp != nullptr)
					*bpp = pixel_format.component_count * 8;
				return FIT_BITMAP;		
			}
		}
		else if (pixel_format.component_type == PixelFormat::TYPE_FLOAT)
		{
			if (pixel_format.component_count == 1)
				return FIT_FLOAT;
			if (pixel_format.component_count == 3)
				return FIT_RGBF;
			if (pixel_format.component_count == 4)
				return FIT_RGBAF;
		}
		return FIT_UNKNOWN;
	}

	ImageDescription ImageTools::GetImageDescription(FIBITMAP * image)
	{
		assert(image != nullptr);

		// test whether we can handle that format
		PixelFormat pixel_format = PixelFormat::FromImage(image);
		if (pixel_format.IsValid())
		{
			ImageDescription result;
			result.width        = FreeImage_GetWidth(image);
			result.height       = FreeImage_GetHeight(image);
			result.pixel_format = pixel_format;
			result.line_size    = FreeImage_GetLine(image);
			result.pitch_size   = FreeImage_GetPitch(image);
			result.padding_size = result.pitch_size - result.line_size;
			result.data         = FreeImage_GetBits(image);

			// test whether the result is valid (line_size & pitch come from FreeImage ... just in case)
			if (result.IsValid())
				return result;				
		}			
		return ImageDescription();
	}

	class ImageToolsCopyPixelsImpl
	{
	public:

		template<typename T>
		static void CopyPixels(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height)
		{
			for (int l = 0 ; l < height ; ++l)
			{
				T const * s = ImageTools::GetPixelAddress<T>(src_desc, src_x, src_y + l);
				T       * d = ImageTools::GetPixelAddress<T>(dst_desc, dst_x, dst_y + l);
				memcpy(d, s, width * sizeof(T));
			}		
		}	

		template<typename T>
		static void CopyPixelsWithCentralSymetry(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height)
		{
			for (int l = 0 ; l < height ; ++l)
			{
				T const * s = ImageTools::GetPixelAddress<T>(src_desc, src_x, src_y + l);
				T       * d = ImageTools::GetPixelAddress<T>(dst_desc, dst_x, dst_y + height - 1 - l);
				for (int c = 0 ; c < width ; ++c)
					d[width - 1 - c] = s[c];
			}		
		}	
	};

	void ImageTools::CopyPixels(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height)
	{
		assert(src_desc.IsValid());
		assert(dst_desc.IsValid());
		assert(src_desc.pixel_format == dst_desc.pixel_format);

		assert(width  >= 0);
		assert(height >= 0);
		assert(src_x >= 0 && src_x + width  <= src_desc.width);
		assert(src_y >= 0 && src_y + height <= src_desc.height);
		assert(dst_x >= 0 && dst_x + width  <= dst_desc.width);
		assert(dst_y >= 0 && dst_y + height <= dst_desc.height);

		PixelFormat const & src_pixel_format = src_desc.pixel_format;

		if (src_pixel_format.component_type == PixelFormat::TYPE_UNSIGNED_CHAR)
		{
			if (src_pixel_format.component_count == 1)
				ImageToolsCopyPixelsImpl::CopyPixels<PixelGray>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_pixel_format.component_count == 3)
				ImageToolsCopyPixelsImpl::CopyPixels<PixelBGR>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_pixel_format.component_count == 4)
				ImageToolsCopyPixelsImpl::CopyPixels<PixelBGRA>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);		
		}
		else if (src_pixel_format.component_type == PixelFormat::TYPE_FLOAT)
		{
			if (src_pixel_format.component_count == 1)
				ImageToolsCopyPixelsImpl::CopyPixels<PixelGrayFloat>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_pixel_format.component_count == 3)
				ImageToolsCopyPixelsImpl::CopyPixels<PixelRGBFloat>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_pixel_format.component_count == 4)
				ImageToolsCopyPixelsImpl::CopyPixels<PixelRGBAFloat>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);		
		}
	}

	void ImageTools::CopyPixelsWithCentralSymetry(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height)
	{
		assert(src_desc.IsValid());
		assert(dst_desc.IsValid());
		assert(src_desc.pixel_format == dst_desc.pixel_format);

		assert(width  >= 0);
		assert(height >= 0);
		assert(src_x >= 0 && src_x + width  <= src_desc.width);
		assert(src_y >= 0 && src_y + height <= src_desc.height);
		assert(dst_x >= 0 && dst_x + width  <= dst_desc.width);
		assert(dst_y >= 0 && dst_y + height <= dst_desc.height);

		PixelFormat const & src_pixel_format = src_desc.pixel_format;

		if (src_pixel_format.component_type == PixelFormat::TYPE_UNSIGNED_CHAR)
		{
			if (src_pixel_format.component_count == 1)
				ImageToolsCopyPixelsImpl::CopyPixelsWithCentralSymetry<PixelGray>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_pixel_format.component_count == 3)
				ImageToolsCopyPixelsImpl::CopyPixelsWithCentralSymetry<PixelBGR>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_pixel_format.component_count == 4)
				ImageToolsCopyPixelsImpl::CopyPixelsWithCentralSymetry<PixelBGRA>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);		
		}
		else if (src_pixel_format.component_type == PixelFormat::TYPE_FLOAT)
		{
			if (src_pixel_format.component_count == 1)
				ImageToolsCopyPixelsImpl::CopyPixelsWithCentralSymetry<PixelGrayFloat>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_pixel_format.component_count == 3)
				ImageToolsCopyPixelsImpl::CopyPixelsWithCentralSymetry<PixelRGBFloat>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);
			else if (src_pixel_format.component_count == 4)
				ImageToolsCopyPixelsImpl::CopyPixelsWithCentralSymetry<PixelRGBAFloat>(src_desc, dst_desc, src_x, src_y, dst_x, dst_y, width, height);		
		}
	}

	bool ImageTools::IsGrayscaleImage(FIBITMAP * image)
	{
		assert(image != nullptr);

		// ignore unhandled message
		ImageDescription desc = ImageTools::GetImageDescription(image);
		if (!desc.IsValid())
			return false;

		// multiple components => not grayscale
		if (desc.pixel_format.component_count != 1)
			return false;

		// a 'luminance' image is a grayscale
		if (desc.pixel_format.component_type == PixelFormat::TYPE_FLOAT)
			return true;

		// 1 component of type UNSIGNED CHAR :
		//   - can be an index on a palette
		//   - can be a 'luminance' value

		RGBQUAD * palette = FreeImage_GetPalette(image); // luminance ?
		if (palette == nullptr)
			return true;

		// test whether the palette is a grayscale palette
		unsigned int color_count = FreeImage_GetColorsUsed(image);
		if (color_count != 256)
			return false;

		for (unsigned int i = 0 ; i < color_count ; ++i)
		{
			if (palette[i].rgbRed != (BYTE)i)
				return false;
			if (palette[i].rgbGreen != (BYTE)i)
				return false;
			if (palette[i].rgbBlue != (BYTE)i)
				return false;
		}
		return true;
	}

	FIBITMAP * ImageTools::LoadImageFromBuffer(Buffer<char> buffer)
	{
		FIBITMAP * result = nullptr;

		if (buffer != nullptr)
		{
			FIMEMORY * memory = FreeImage_OpenMemory((BYTE*)buffer.data, (DWORD)buffer.bufsize);
			if (memory != nullptr)
			{
				FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(memory, 0);

				result = FreeImage_LoadFromMemory(format, memory, 0);

				if (FreeImage_GetImageType(result) == FIT_BITMAP && FreeImage_GetBPP(result) == 8)
				{
					if (!IsGrayscaleImage(result)) // don't want a palette any more
					{
						FIBITMAP * other = FreeImage_ConvertTo32Bits(result);
						FreeImage_Unload(result);
						result = other;           // this code is good even if the conversion fails 
					}
				}

				FreeImage_CloseMemory(memory);
			}
		}	
		return result;	
	}

	FIBITMAP * ImageTools::LoadImageFromFile(char const * filename)
	{
		assert(filename != nullptr);
		return LoadImageFromBuffer(FileTools::LoadFile(filename, false));
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
