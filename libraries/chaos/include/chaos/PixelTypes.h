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

		/** constructor */
		PixelBGR() = default;
		/** copy constructor */
		PixelBGR(PixelBGR const & src) = default;
		/** conversion */
		PixelBGR(unsigned int color) { operator = (color); }

		/** assignation */
		PixelBGR & operator = (unsigned int color)
		{
			B = (color >> 0) & 0xFF;
			G = (color >> 8) & 0xFF;
			R = (color >> 16) & 0xFF;
			return *this;
		}

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

		/** constructor */
		PixelBGRA() = default;
		/** copy constructor */
		PixelBGRA(PixelBGRA const & src) = default;
		/** conversion */
		PixelBGRA(unsigned int color){ operator = (color); }

		/** assignation */
		PixelBGRA & operator = (unsigned int color)
		{
			B = (color >> 0) & 0xFF;
			G = (color >> 8) & 0xFF;
			R = (color >> 16) & 0xFF;
			A = (color >> 24) & 0xFF;
			return *this;
		}

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

		/** constructor */
		PixelRGBFloat() = default;
		/** copy constructor */
		PixelRGBFloat(PixelRGBFloat const & src) = default;
		/** conversion */
		PixelRGBFloat(glm::vec3 const & color) { operator = (color); }

		/** assignation */
		PixelRGBFloat & operator = (glm::vec3 const & color)
		{
			B = color.b;
			G = color.g;
			R = color.r;
			return *this;
		}
		/** conversion */
		operator glm::vec3() const
		{
			glm::vec3 result;
			result.b = B;
			result.g = G;
			result.r = R;
			return result;
		}

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

		/** constructor */
		PixelRGBAFloat() = default;
		/** copy constructor */
		PixelRGBAFloat(PixelRGBAFloat const & src) = default;
		/** conversion */
		PixelRGBAFloat(glm::vec4 const & color) { operator = (color); }

		/** automatic conversion */
		PixelRGBAFloat & operator = (glm::vec4 const & color)
		{
			B = color.b;
			G = color.g;
			R = color.r;
			A = color.a;
			return *this;
		}

		operator glm::vec4() const
		{
			glm::vec4 result;
			result.b = B;
			result.g = G;
			result.r = R;
			result.a = A;
			return result;
		}

		float R;
		float G;
		float B;
		float A;
	};

	/**
	* PixelDepthStencil : a structure that helps depth24+stencil8 pixels
	**/

	class PixelDepthStencil // FreeImage produce RGBA pixels by default (for float) !!!
	{
	public:

		/** constructor */
		PixelDepthStencil() = default;
		/** copy constructor */
		PixelDepthStencil(PixelDepthStencil const & src) = default;

		unsigned char special1;
		unsigned char special2;
		unsigned char special3;
		unsigned char special4;
	};

	/**
	* PixelTypes : the list of all supported pixel classes (XXX : order matters)
	*/

	using PixelTypes = boost::mpl::vector<PixelGray, PixelBGR, PixelBGRA, PixelGrayFloat, PixelRGBFloat, PixelRGBAFloat, PixelDepthStencil>;

	/**
	* Class to make basic pixel component conversions
	*/

	class PixelComponentConverter
	{
	public:

		/// conversion from 'unsigned char' template : no implementation
		template<typename T>
		static inline T Convert(unsigned char src);
		/// conversion from 'float' template : no implementation
		template<typename T>
		static inline T Convert(float src);
		/// conversion from 'unsigned char' specialization for return type
		template<>
		static inline unsigned char Convert<unsigned char>(unsigned char src){ return src;}
		/// conversion from 'unsigned char' specialization for return type
		template<>
		static inline float Convert<float>(float src){ return src;}
		/// conversion from 'float' specialization for return type
		template<>
		static inline unsigned char Convert<unsigned char>(float src){ return (unsigned char)min(255.0f, 255.0f * src);}
		/// conversion from 'float' specialization for return type
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

		/// nop 'conversion' method
		template<typename T>
		static inline void Convert(T & dst, T const & src) { dst = src; }

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
		static inline void Convert(PixelGray & dst, PixelDepthStencil const & src)
		{
			assert(0);
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
		static inline void Convert(PixelGrayFloat & dst, PixelDepthStencil const & src)
		{
			assert(0);
		}

		/// conversion to 'PixelBGR' methods
		static inline void Convert(PixelBGR & dst, PixelGray const & src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<unsigned char>(src);
		}
		static inline void Convert(PixelBGR & dst, PixelGrayFloat const & src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<unsigned char>(src);
		}
		static inline void Convert(PixelBGR & dst, PixelBGRA const & src)
		{
			dst.B = PixelComponentConverter::Convert<unsigned char>(src.B);
			dst.G = PixelComponentConverter::Convert<unsigned char>(src.G);
			dst.R = PixelComponentConverter::Convert<unsigned char>(src.R);
		}
		static inline void Convert(PixelBGR & dst, PixelRGBFloat const & src)
		{
			dst.B = PixelComponentConverter::Convert<unsigned char>(src.B);
			dst.G = PixelComponentConverter::Convert<unsigned char>(src.G);
			dst.R = PixelComponentConverter::Convert<unsigned char>(src.R);
		}
		static inline void Convert(PixelBGR & dst, PixelRGBAFloat const & src)
		{
			dst.B = PixelComponentConverter::Convert<unsigned char>(src.B);
			dst.G = PixelComponentConverter::Convert<unsigned char>(src.G);
			dst.R = PixelComponentConverter::Convert<unsigned char>(src.R);
		}
		static inline void Convert(PixelBGR & dst, PixelDepthStencil const & src)
		{
			assert(0);
		}

		/// conversion to 'PixelBGRA' methods
		static inline void Convert(PixelBGRA & dst, PixelGray const & src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<unsigned char>(src);
			dst.A = 255;
		}
		static inline void Convert(PixelBGRA & dst, PixelGrayFloat const & src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<unsigned char>(src);
			dst.A = 255;
		}
		static inline void Convert(PixelBGRA & dst, PixelBGR const & src)
		{
			dst.B = PixelComponentConverter::Convert<unsigned char>(src.B);
			dst.G = PixelComponentConverter::Convert<unsigned char>(src.G);
			dst.R = PixelComponentConverter::Convert<unsigned char>(src.R);
			dst.A = 255;
		}
		static inline void Convert(PixelBGRA & dst, PixelRGBFloat const & src)
		{
			dst.B = PixelComponentConverter::Convert<unsigned char>(src.B);
			dst.G = PixelComponentConverter::Convert<unsigned char>(src.G);
			dst.R = PixelComponentConverter::Convert<unsigned char>(src.R);
			dst.A = 255;
		}
		static inline void Convert(PixelBGRA & dst, PixelRGBAFloat const & src)
		{
			dst.B = PixelComponentConverter::Convert<unsigned char>(src.B);
			dst.G = PixelComponentConverter::Convert<unsigned char>(src.G);
			dst.R = PixelComponentConverter::Convert<unsigned char>(src.R);
			dst.A = PixelComponentConverter::Convert<unsigned char>(src.A);
		}
		static inline void Convert(PixelBGRA & dst, PixelDepthStencil const & src)
		{
			assert(0);
		}

		/// conversion to 'PixelRGBFloat' methods
		static inline void Convert(PixelRGBFloat & dst, PixelGray const & src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<float>(src);
		}
		static inline void Convert(PixelRGBFloat & dst, PixelGrayFloat const & src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<float>(src);
		}
		static inline void Convert(PixelRGBFloat & dst, PixelBGR const & src)
		{
			dst.R = PixelComponentConverter::Convert<float>(src.R);
			dst.G = PixelComponentConverter::Convert<float>(src.G);
			dst.B = PixelComponentConverter::Convert<float>(src.B);					
		}
		static inline void Convert(PixelRGBFloat & dst, PixelBGRA const & src)
		{
			dst.R = PixelComponentConverter::Convert<float>(src.R);
			dst.G = PixelComponentConverter::Convert<float>(src.G);
			dst.B = PixelComponentConverter::Convert<float>(src.B);
		}
		static inline void Convert(PixelRGBFloat & dst, PixelRGBAFloat const & src)
		{
			dst.R = PixelComponentConverter::Convert<float>(src.R);
			dst.G = PixelComponentConverter::Convert<float>(src.G);
			dst.B = PixelComponentConverter::Convert<float>(src.B);
		}
		static inline void Convert(PixelRGBFloat & dst, PixelDepthStencil const & src)
		{
			assert(0);
		}

		/// conversion to 'PixelRGBAFloat' methods
		static inline void Convert(PixelRGBAFloat & dst, PixelGray const & src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<float>(src);
			dst.A = 1.0f;
		}
		static inline void Convert(PixelRGBAFloat & dst, PixelGrayFloat const & src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<float>(src);
			dst.A = 1.0f;
		}
		static inline void Convert(PixelRGBAFloat & dst, PixelBGR const & src)
		{
			dst.R = PixelComponentConverter::Convert<float>(src.R);
			dst.G = PixelComponentConverter::Convert<float>(src.G);
			dst.B = PixelComponentConverter::Convert<float>(src.B);
			dst.A = 1.0f;
		}
		static inline void Convert(PixelRGBAFloat & dst, PixelBGRA const & src)
		{
			dst.R = PixelComponentConverter::Convert<float>(src.R);
			dst.G = PixelComponentConverter::Convert<float>(src.G);
			dst.B = PixelComponentConverter::Convert<float>(src.B);
			dst.A = PixelComponentConverter::Convert<float>(src.A);
		}
		static inline void Convert(PixelRGBAFloat & dst, PixelRGBFloat const & src)
		{
			dst.R = PixelComponentConverter::Convert<float>(src.R);
			dst.G = PixelComponentConverter::Convert<float>(src.G);
			dst.B = PixelComponentConverter::Convert<float>(src.B);
			dst.A = 1.0f;
		}
		static inline void Convert(PixelRGBAFloat & dst, PixelDepthStencil const & src)
		{
			assert(0);
		}

		/// conversion to 'PixelDepthStencil' methods
		static inline void Convert(PixelDepthStencil & dst, PixelGrayFloat const & src)
		{
			assert(0);
		}
		static inline void Convert(PixelDepthStencil & dst, PixelBGR const & src)
		{
			assert(0);
		}
		static inline void Convert(PixelDepthStencil & dst, PixelBGRA const & src)
		{
			assert(0);
		}
		static inline void Convert(PixelDepthStencil & dst, PixelRGBFloat const & src)
		{
			assert(0);
		}
		static inline void Convert(PixelDepthStencil & dst, PixelRGBAFloat const & src)
		{
			assert(0);
		}
	};

}; // namespace chaos

