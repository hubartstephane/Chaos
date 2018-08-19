#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ImageDescription.h>
#include <chaos/Buffer.h>
#include <chaos/FilePath.h>

namespace chaos
{
	/** 
	* ImageTools : deserve to load some images
	*/

	class ImageTools
	{

	public:

		/** fill the color of the background */
		static void FillImageBackground(ImageDescription & image_description, glm::vec4 const & color);

		/** generate a free image corresponding to a given pixel format */
		static FIBITMAP * GenFreeImage(PixelFormat const & pixel_format, int width, int height);
		/** get the free image description frm a type */
		static FREE_IMAGE_TYPE GetFreeImageType(PixelFormat const & pixel_format, int * bpp = nullptr);
		/** the free image prefered file format for a given PixelFormat */
		static FREE_IMAGE_FORMAT GetFreeImageFormat(PixelFormat const & pixel_format);

		/** load an image from a buffer */
		static FIBITMAP * LoadImageFromBuffer(Buffer<char> buffer);
		/** load an image from file (use our own implementation instead of FreeImage_LoadFromFile to provide our own error management) */
		static FIBITMAP * LoadImageFromFile(FilePathParam const & path);

		/** load multiple image from a file (animated gif) */
		static FIMULTIBITMAP * LoadMultiImageFromFile(FilePathParam const & path);
		/** load multiple image from a file (animated gif) */
		static FIMULTIBITMAP * LoadMultiImageFromFile(Buffer<char> buffer);

		/** returns true whether the image is paletted, 8 bits and its color are grays */
		static bool IsGrayscaleImage(FIBITMAP * image, bool * alpha_needed = nullptr);
		/** transform input image into a supported format whenever possible (may destroy the input in case of failure) */
		static FIBITMAP * ConvertToSupportedType(FIBITMAP * src, bool can_delete_src);

		/** get the image information from a FreeImage */
		static ImageDescription GetImageDescription(FIBITMAP * image);
		/** get pixel format corresponding to an image */
		static PixelFormat GetPixelFormat(FIBITMAP * image);

		/** copy pixels */
		static void CopyPixels(ImageDescription const & src_desc, ImageDescription & dst_desc, int src_x, int src_y, int dst_x, int dst_y, int width, int height, bool central_symetry);
		/** convert image into another pixel format + central symetry if required */
		static ImageDescription ConvertPixels(ImageDescription const & src_desc, PixelFormat const & final_pixel_format, char * conversion_buffer, bool central_symetry);

		/** create a ImageTexture with DWORD alignment requirements with a given buffer */
		static ImageDescription GetImageDescriptionForAlignedTexture(PixelFormat const & pixel_format, int width, int height, char * buffer);
		/** compute the memory requirement for a texture with DWORD aligned rows texture */
		static int GetMemoryRequirementForAlignedTexture(PixelFormat const & pixel_format, int width, int height);

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
		static FIBITMAP * GenFreeImage(int width, int height, GENERATOR const & generator)
		{
			FIBITMAP * result = GenFreeImage(PixelFormat::GetPixelFormat<T>(), width, height);
			if (result != nullptr)
			{
				ImageDescription image_desc = GetImageDescription(result);
				generator(image_desc);			
			}
			return result;
		}
	};

}; // namespace chaos

