#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>

namespace chaos
{
	// FreeImage produces
	// 
	//   - BGR(A) pixels for UNSIGNED CHAR images
	//   - RGB(A) pixels for FLOAT images
	//
	// The difference come from endianess. That have no signification for FLOAT but for 32bits encoded values it has !
	//

	/**
	* PixelGray : a grayscale pixel
	**/

	using PixelGray = unsigned char;

	/**
	* PixelBGR : a structure that helps manipulating 24 bit pixel 
	**/

	class PixelBGR // FreeImage produce BGR pixels by default (for unsigned char)
	{
	public:
		unsigned char B;
		unsigned char G;
		unsigned char R;
	};

	/**
	* PixelBGRA : a structure that helps manipulating 32 bit pixel
	**/

	class PixelBGRA // FreeImage produce BGRA pixels by default (for unsigned char)
	{
	public:
		unsigned char B;
		unsigned char G;
		unsigned char R;
		unsigned char A;
	};

	/**
	* PixelGrayFloat : a grayscale pixel (float data)
	**/

	using PixelGrayFloat = float;

	/**
	* PixelRGBFloat : a structure that helps manipulating 3 x float pixels
	**/

	class PixelRGBFloat // FreeImage produce RGB pixels by default (for float) !!!
	{
	public:
		float R;
		float G;
		float B;
	};

	/**
	* PixelRGBAFloat : a structure that helps manipulating 4 x float pixels
	**/

	class PixelRGBAFloat // FreeImage produce RGBA pixels by default (for float) !!!
	{
	public:
		float R;
		float G;
		float B;
		float A;
	};

	/** 
	* PixelFormat : the accepted pixel formats
	*/

	class PixelFormat
	{
	public:

		/** the type of each component of each pixels */
		static int const TYPE_UNKNOWN = 0;
		static int const TYPE_UNSIGNED_CHAR = 1;
		static int const TYPE_FLOAT = 2;

	public:

		/** constructor */
		PixelFormat() = default;
		
		PixelFormat(PixelFormat const & other) = default;

		PixelFormat(int in_component_type, int in_component_count) : 
			component_type(in_component_type), 
			component_count(in_component_count){};

		/** get the size of one pixel */
		int GetPixelSize() const;
		/** returns true whether the pixel format is handled */
		bool IsValid() const;
		/** returns true whether the pixel format are same */
		bool operator == (PixelFormat const & other) const;
		/** returns true whether the pixel format are different */
		bool operator != (PixelFormat const & other) const;
		/** gets the BPP if applyable (non float texture) */
		int GetBPP() const;

		/** get pixel format corresponding to an image */
		static PixelFormat FromImageDescription(FIBITMAP * image);

		/** transform a type into pixel format (component type and count) */
		template<typename T>
		static PixelFormat GetPixelFormat();

		template<>
		static PixelFormat GetPixelFormat<PixelGray>(){ return PixelFormat(TYPE_UNSIGNED_CHAR, 1);}
		template<>
		static PixelFormat GetPixelFormat<PixelBGR>(){ return PixelFormat(TYPE_UNSIGNED_CHAR, 3);}
		template<>
		static PixelFormat GetPixelFormat<PixelBGRA>(){ return PixelFormat(TYPE_UNSIGNED_CHAR, 4);}
		template<>
		static PixelFormat GetPixelFormat<PixelGrayFloat>(){ return PixelFormat(TYPE_FLOAT, 1);}
		template<>
		static PixelFormat GetPixelFormat<PixelRGBFloat>(){ return PixelFormat(TYPE_FLOAT, 3);}
		template<>
		static PixelFormat GetPixelFormat<PixelRGBAFloat>(){ return PixelFormat(TYPE_FLOAT, 4);}

	public:

		/** the type of the components */
		int component_type{ TYPE_UNKNOWN };
		/** the number of components for each pixels */
		int component_count{ 0 };		
	};

	/** 
	* ImageFaceDescription : data to work with pixels
	*/

	class ImageDescription
	{
	public:

		/** constructor */
		ImageDescription() = default;

		ImageDescription(void * in_data, int in_width, int in_height, PixelFormat const & in_pixel_format, int in_padding = 0);

		/** returns true whether the description is valid */
		bool IsValid() const;
		/** returns true whether the image is empty */
		bool IsEmpty() const;
		/** get the image information for a sub image */
		ImageDescription GetSubImageDescription(int x, int y, int wanted_width, int wanted_height) const;

	public:

		/** the buffer */
		void * data{ nullptr };
		/** the image width */
		int    width{ 0 };
		/** the image height */
		int    height{ 0 };	
		/** the pixel format */
		PixelFormat pixel_format;
		/** size of line in bytes (exclude padding) : width * pixel_size */
		int    line_size{ 0 };
		/** size of line in bytes (including padding) : padding + line_size */
		int    pitch_size{ 0 };
		/** padding a the end of a line in bytes */
		int    padding_size{ 0 };
	};


	/**
	* TextureArrayGeneratorParams : parameters for merging some slices of different format
	*/

	class xTextureArrayGeneratorParams
	{
	public:

		/** whether grayscale sliced must be converted into color slice */
		bool accept_grayscaled{true};
		/** whether float images must be converted into unsigned char slice */
		bool accept_float{true};
		/** the wanted component count */
		int component_count{0};
		/** the wanted component type */
		int component_type{0};		
	};

	/** 
	* ImageTools : deserve to load some images
	*/

	class ImageTools
	{

	public:

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
	};

}; // namespace chaos

