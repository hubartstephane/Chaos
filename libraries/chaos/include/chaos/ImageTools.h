#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>

namespace chaos
{

	/**
	* PixelGray : a grayscale pixel
	**/

	using PixelGray = unsigned char;

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
	* PixelGrayFloat : a grayscale pixel (float data)
	**/

	using PixelGrayFloat = float;

	/**
	* PixelRGBFloat : a structure that helps manipulating 3 x float pixels
	**/

	class PixelRGBFloat
	{
	public:
		float B;
		float G;
		float R;
	};

	/**
	* PixelRGBAFloat : a structure that helps manipulating 4 x float pixels
	**/

	class PixelRGBAFloat
	{
	public:
		float B;
		float G;
		float R;
		float A;
	};

	/** 
	* ImageFaceDescription : data to work with pixels
	*/

	class ImageDescription
	{
	public:

		/** the type of each component of each pixels */
		static int const TYPE_UNKNOWN = 0;
		static int const TYPE_UNSIGNED_CHAR = 1;
		static int const TYPE_FLOAT = 2;

		/** constructor */
		ImageDescription() = default;

		ImageDescription(void * in_data, int in_width, int in_height, int in_component_type, int in_component_count, int in_padding = 0);

		/** returns true whether the description is valid */
		bool IsValid() const;
		/** returns true whether the image is empty */
		bool IsEmpty() const;
		/** get the size of one component */
		int GetComponentSize() const;
		/** get the size of one pixel */
		int GetPixelSize() const;
		/** returns true whether the pixel format is handled */
		bool IsPixelFormatValid() const;
		/** returns true whether the pixel format are same */
		bool ArePixelFormatSame(ImageDescription const & other) const;
		/** get the image information for a sub image */
		ImageDescription GetSubImageDescription(int x, int y, int wanted_width, int wanted_height) const;

		/** transform a type into pixel format (component type and count) */
		template<typename T>
		static std::pair<int, int> GetPixelFormat(){ return std::make_pair(0, 0);}

		template<>
		static std::pair<int, int> GetPixelFormat<PixelGray>(){ return std::make_pair(TYPE_UNSIGNED_CHAR, 1);}
		template<>
		static std::pair<int, int> GetPixelFormat<PixelRGB>(){ return std::make_pair(TYPE_UNSIGNED_CHAR, 3);}
		template<>
		static std::pair<int, int> GetPixelFormat<PixelRGBA>(){ return std::make_pair(TYPE_UNSIGNED_CHAR, 4);}
		template<>
		static std::pair<int, int> GetPixelFormat<PixelGrayFloat>(){ return std::make_pair(TYPE_FLOAT, 1);}
		template<>
		static std::pair<int, int> GetPixelFormat<PixelRGBFloat>(){ return std::make_pair(TYPE_FLOAT, 3);}
		template<>
		static std::pair<int, int> GetPixelFormat<PixelRGBAFloat>(){ return std::make_pair(TYPE_FLOAT, 4);}

	public:

		/** the buffer */
		void * data{ nullptr };
		/** the image width */
		int    width{ 0 };
		/** the image height */
		int    height{ 0 };
		/** the type of the components */
		int    component_type{ 0 };
		/** the number of components for each pixels */
		int    component_count{ 0 };		
		/** size of line in bytes (exclude padding) : width * pixel_size */
		int    line_size{ 0 };
		/** size of line in bytes (including padding) : padding + line_size */
		int    pitch_size{ 0 };
		/** padding a the end of a line in bytes */
		int    padding_size{ 0 };
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
		static bool IsGreyscaleImage(FIBITMAP * image);

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

