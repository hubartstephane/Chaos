#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ImageDescription.h>
#include <chaos/Buffer.h>

namespace chaos
{
	/** 
	* ImageTools : deserve to load some images
	*/

	class ImageTools
	{

	public:

		/** get the free image description frm a type */
		static FREE_IMAGE_TYPE GetFreeImageType(PixelFormat const & pixel_format, int * bpp = nullptr);

		/** load an image from a buffer */
		static FIBITMAP * LoadImageFromBuffer(Buffer<char> buffer);
		/** load an image from file (use our own implementation instead of FreeImage_LoadFromFile to provide our own error management) */
		static FIBITMAP * LoadImageFromFile(char const * filename);
		/** load multiple image from a file (animated gif) */
		static FIMULTIBITMAP * LoadMultiImageFromFile(char const * filename);

		/** returns true whether the image is paletted, 8 bits and its color are grays */
		static bool IsGrayscaleImage(FIBITMAP * image);

		/** get the image information from a FreeImage */
		static ImageDescription GetImageDescription(FIBITMAP * image);
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

		/** Generate an image from lambda */
		template<typename T, typename GENERATOR>
		static FIBITMAP * GenImage(int width, int height, GENERATOR const & generator)
		{
			FIBITMAP * result = nullptr;

			// get corresponding pixel format
			PixelFormat pixel_format = PixelFormat::GetPixelFormat<T>();
			if (!pixel_format.IsValid)
				return result;

			// get freeimage format
			int bpp = 0;
			FREE_IMAGE_TYPE image_type = GetFreeImageType(pixel_format, &bpp);
			if (image_type == FIT_UNKNOWN)
				return nullptr;

			// allocate the freeimage
			result = FreeImage_Allocate(image_type, width, height, bpp);
			if (result == nullptr)
				return result;

			// generate the pixels
			ImageDescription image_desc = GetImageDescription(result);
			generator(image_desc, (T*)image_desc.data);

			return result;
		}
	};

}; // namespace chaos

