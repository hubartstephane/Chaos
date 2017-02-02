#pragma once

#include <chaos/StandardHeaders.h>

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

		/** the well know format */
		static int const FORMAT_UNKNOWN    = 0;
		static int const FORMAT_GRAY       = 1;
		static int const FORMAT_RGB        = 2;
		static int const FORMAT_RGBA       = 3;
		static int const FORMAT_GRAY_FLOAT = 4;
		static int const FORMAT_RGB_FLOAT  = 5;
		static int const FORMAT_RGBA_FLOAT = 6;

	public:

		/** constructor */
		PixelFormat() = default;
		
		PixelFormat(PixelFormat const & other) = default;

		PixelFormat(int in_component_type, int in_component_count) : 
			component_type(in_component_type), 
			component_count(in_component_count){};

		PixelFormat(int in_format);

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

}; // namespace chaos

