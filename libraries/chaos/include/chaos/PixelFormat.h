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
	 * Class to make basic pixel component conversions
	 */

	class PixelComponentConverter
	{
	public:

		template<typename T>
		static inline T Convert(unsigned char src);

		template<typename T>
		static inline T Convert(float src);

		template<>
		static inline unsigned char Convert<unsigned char>(unsigned char src){ return src;}

		template<>
		static inline float Convert<float>(float src){ return src;}

		template<>
		static inline unsigned char Convert<unsigned char>(float src){ return (unsigned char)(255.0f * src);}

		template<>
		static inline float Convert<float>(unsigned char src){ return ((float)src) / 255.0f;}
	};
	
	// 3 methods for grayscale conversion : www.johndcook.com/blog/2009/08/24/algorithms-convert-color-grayscale
	//
	//   lightness :   (max(R, G, B) + min(R, G, B)) / 2
	//
	//   average   :   (R + G + B) / 3
	//
	//   luminosity :  (0.21 R + 0.72 G + 0.07 B)
	//
	// => we select the 'average' method because it is faster (and does not use float values)

	class PixelConverter
	{
	public:
		
		/// null 'conversion' method
		template<typename T>
		static inline void Convert(T & dst, T const & src){ dst = src;}

		/// conversion to 'PixelGray' methods
		static inline void Convert(PixelGray & dst, PixelGrayFloat const & src)
		{
			dst = PixelComponentConverter::Convert<unsigned char>(src);		
		}
		static inline void Convert(PixelGray & dst, PixelBGR const & src)
		{
			dst = PixelComponentConverter::Convert<unsigned char>((unsigned char)((src.R + src.G + src.B) / 3));
		}
		static inline void Convert(PixelGray & dst, PixelBGRA const & src)
		{
			dst = PixelComponentConverter::Convert<unsigned char>((unsigned char)((src.R + src.G + src.B) / 3));
		}
		static inline void Convert(PixelGray & dst, PixelRGBFloat const & src)
		{
			dst = PixelComponentConverter::Convert<unsigned char>((src.R + src.G + src.B) / 3.0f);
		}
		static inline void Convert(PixelGray & dst, PixelRGBAFloat const & src)
		{
			dst = PixelComponentConverter::Convert<unsigned char>((src.R + src.G + src.B) / 3.0f);
		}

		/// conversion to 'PixelGrayFloat' methods
		static inline void Convert(PixelGrayFloat & dst, PixelGray const & src)
		{
			dst = PixelComponentConverter::Convert<float>(src);		
		}
		static inline void Convert(PixelGrayFloat & dst, PixelBGR const & src)
		{
			dst = PixelComponentConverter::Convert<float>((unsigned char)((src.R + src.G + src.B) / 3));
		}
		static inline void Convert(PixelGrayFloat & dst, PixelBGRA const & src)
		{
			dst = PixelComponentConverter::Convert<float>((unsigned char)((src.R + src.G + src.B) / 3));
		}
		static inline void Convert(PixelGrayFloat & dst, PixelRGBFloat const & src)
		{
			dst = PixelComponentConverter::Convert<float>((src.R + src.G + src.B) / 3.0f);
		}
		static inline void Convert(PixelGrayFloat & dst, PixelRGBAFloat const & src)
		{
			dst = PixelComponentConverter::Convert<float>((src.R + src.G + src.B) / 3.0f);
		}



	};






#if 0






	static inline void Convert(PixelGrayFloat & dst, PixelGray const & src)
	{	
	}
	static inline void Convert(PixelGrayFloat & dst, PixelGrayFloat const & src)
	{	

	}
	static inline void Convert(PixelGrayFloat & dst, PixelBGR const & src)
	{

	}
	static inline void Convert(PixelGrayFloat & dst, PixelBGRA const & src)
	{

	}
	static inline void Convert(PixelGrayFloat & dst, PixelRGBFloat const & src)
	{

	}
	static inline void Convert(PixelGrayFloat & dst, PixelRGBAFloat const & src)
	{
		
	}















	static inline void Convert(PixelBGR & dst, PixelGray const & src)
	{
		
	}
	static inline void Convert(PixelBGR & dst, PixelGrayFloat const & src)
	{	

	}
	static inline void Convert(PixelBGR & dst, PixelBGR const & src)
	{
		dst = src;
	}
	static inline void Convert(PixelBGR & dst, PixelBGRA const & src)
	{

	}
	static inline void Convert(PixelBGR & dst, PixelRGBFloat const & src)
	{

	}
	static inline void Convert(PixelBGR & dst, PixelRGBAFloat const & src)
	{

	}


	static inline void Convert(PixelBGRA & dst, PixelGray const & src)
	{	
	}
	static inline void Convert(PixelBGRA & dst, PixelGrayFloat const & src)
	{	

	}
	static inline void Convert(PixelBGRA & dst, PixelBGR const & src)
	{

	}
	static inline void Convert(PixelBGRA & dst, PixelBGRA const & src)
	{
		dst = src;
	}
	static inline void Convert(PixelBGRA & dst, PixelRGBFloat const & src)
	{

	}
	static inline void Convert(PixelBGRA & dst, PixelRGBAFloat const & src)
	{

	}




	static inline void Convert(PixelRGBFloat & dst, PixelGray const & src)
	{	
	}
	static inline void Convert(PixelRGBFloat & dst, PixelGrayFloat const & src)
	{	

	}
	static inline void Convert(PixelRGBFloat & dst, PixelBGR const & src)
	{

	}
	static inline void Convert(PixelRGBFloat & dst, PixelBGRA const & src)
	{

	}
	static inline void Convert(PixelRGBFloat & dst, PixelRGBFloat const & src)
	{
		dst = src;
	}
	static inline void Convert(PixelRGBFloat & dst, PixelRGBAFloat const & src)
	{

	}

	static inline void Convert(PixelRGBAFloat & dst, PixelGray const & src)
	{	
	}
	static inline void Convert(PixelRGBAFloat & dst, PixelGrayFloat const & src)
	{	

	}
	static inline void Convert(PixelRGBAFloat & dst, PixelBGR const & src)
	{

	}
	static inline void Convert(PixelRGBAFloat & dst, PixelBGRA const & src)
	{

	}
	static inline void Convert(PixelRGBAFloat & dst, PixelRGBFloat const & src)
	{

	}
	static inline void Convert(PixelRGBAFloat & dst, PixelRGBAFloat const & src)
	{
		dst = src;
	}










#endif


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

		/** the well known formats */
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
		static PixelFormat FromImage(FIBITMAP * image);

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
	 * PixelFormatMergeParams
	 */

	class PixelFormatMergeParams
	{
	
	public:

		/** if valid, the resulting pixel format should be set to this */
		PixelFormat pixel_format;
		/** if set to false, all luminances formats will become RGB */
		bool accept_luminance{true};
		/** if set to false, all float formats will become unsigned char */
		bool accept_float{true};	
	};

	/**
	* PixelFormatMerger : utility function to find a common PixelFormat for multiple images
	*/

	class PixelFormatMerger
	{
	
	public:

		/** constructor */
		PixelFormatMerger(PixelFormatMergeParams const & in_params = PixelFormatMergeParams());

		/** reset the merger */
		void Reset(PixelFormatMergeParams const & in_params = PixelFormatMergeParams());
		/** the merge method */
		void Merge(PixelFormat src);
		/** get the result if available */
		bool GetResult(PixelFormat & value) const;

	protected:

		/** the result */
		PixelFormat result;
		/** whether a valid result is available */
		bool result_is_available{false};
		/** the parameter for the merge */
		PixelFormatMergeParams params;
	};

}; // namespace chaos

