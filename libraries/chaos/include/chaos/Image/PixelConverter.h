namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class PixelComponentConverter;
	class PixelConverter;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Class to make basic pixel component conversions
	*/

	class CHAOS_API PixelComponentConverter
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
		static inline unsigned char Convert<unsigned char>(unsigned char src) { return src; }
		/// conversion from 'unsigned char' specialization for return type
		template<>
		static inline float Convert<float>(float src) { return src; }
		/// conversion from 'float' specialization for return type
		template<>
		static inline unsigned char Convert<unsigned char>(float src) { return (unsigned char)std::min(255.0f, 255.0f * src); }
		/// conversion from 'float' specialization for return type
		template<>
		static inline float Convert<float>(unsigned char src) { return ((float)src) / 255.0f; }
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

	class CHAOS_API PixelConverter
	{
	public:

		/// nop 'conversion' method
		template<typename T>
		static inline void Convert(T& dst, T const& src) { dst = src; }

		/// conversion to 'PixelGray' methods
		static inline void Convert(PixelGray& dst, PixelGrayFloat const& src)
		{
			dst = PixelComponentConverter::Convert<unsigned char>(src);
		}
		static inline void Convert(PixelGray& dst, PixelBGR const& src)
		{
			dst = PixelComponentConverter::Convert<unsigned char>((unsigned char)((src.R + src.G + src.B) / 3));
		}
		static inline void Convert(PixelGray& dst, PixelBGRA const& src)
		{
			dst = PixelComponentConverter::Convert<unsigned char>((unsigned char)((src.R + src.G + src.B) / 3));
		}
		static inline void Convert(PixelGray& dst, PixelRGBFloat const& src)
		{
			dst = PixelComponentConverter::Convert<unsigned char>((src.R + src.G + src.B) / 3.0f);
		}
		static inline void Convert(PixelGray& dst, PixelRGBAFloat const& src)
		{
			dst = PixelComponentConverter::Convert<unsigned char>((src.R + src.G + src.B) / 3.0f);
		}
		static inline void Convert(PixelGray& dst, PixelDepthStencil const& src)
		{
			assert(0);
		}

		/// conversion to 'PixelGrayFloat' methods
		static inline void Convert(PixelGrayFloat& dst, PixelGray const& src)
		{
			dst = PixelComponentConverter::Convert<float>(src);
		}
		static inline void Convert(PixelGrayFloat& dst, PixelBGR const& src)
		{
			dst = PixelComponentConverter::Convert<float>((unsigned char)((src.R + src.G + src.B) / 3));
		}
		static inline void Convert(PixelGrayFloat& dst, PixelBGRA const& src)
		{
			dst = PixelComponentConverter::Convert<float>((unsigned char)((src.R + src.G + src.B) / 3));
		}
		static inline void Convert(PixelGrayFloat& dst, PixelRGBFloat const& src)
		{
			dst = PixelComponentConverter::Convert<float>((src.R + src.G + src.B) / 3.0f);
		}
		static inline void Convert(PixelGrayFloat& dst, PixelRGBAFloat const& src)
		{
			dst = PixelComponentConverter::Convert<float>((src.R + src.G + src.B) / 3.0f);
		}
		static inline void Convert(PixelGrayFloat& dst, PixelDepthStencil const& src)
		{
			assert(0);
		}

		/// conversion to 'PixelBGR' methods
		static inline void Convert(PixelBGR& dst, PixelGray const& src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<unsigned char>(src);
		}
		static inline void Convert(PixelBGR& dst, PixelGrayFloat const& src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<unsigned char>(src);
		}
		static inline void Convert(PixelBGR& dst, PixelBGRA const& src)
		{
			dst.B = PixelComponentConverter::Convert<unsigned char>(src.B);
			dst.G = PixelComponentConverter::Convert<unsigned char>(src.G);
			dst.R = PixelComponentConverter::Convert<unsigned char>(src.R);
		}
		static inline void Convert(PixelBGR& dst, PixelRGBFloat const& src)
		{
			dst.B = PixelComponentConverter::Convert<unsigned char>(src.B);
			dst.G = PixelComponentConverter::Convert<unsigned char>(src.G);
			dst.R = PixelComponentConverter::Convert<unsigned char>(src.R);
		}
		static inline void Convert(PixelBGR& dst, PixelRGBAFloat const& src)
		{
			dst.B = PixelComponentConverter::Convert<unsigned char>(src.B);
			dst.G = PixelComponentConverter::Convert<unsigned char>(src.G);
			dst.R = PixelComponentConverter::Convert<unsigned char>(src.R);
		}
		static inline void Convert(PixelBGR& dst, PixelDepthStencil const& src)
		{
			assert(0);
		}

		/// conversion to 'PixelBGRA' methods
		static inline void Convert(PixelBGRA& dst, PixelGray const& src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<unsigned char>(src);
			dst.A = 255;
		}
		static inline void Convert(PixelBGRA& dst, PixelGrayFloat const& src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<unsigned char>(src);
			dst.A = 255;
		}
		static inline void Convert(PixelBGRA& dst, PixelBGR const& src)
		{
			dst.B = PixelComponentConverter::Convert<unsigned char>(src.B);
			dst.G = PixelComponentConverter::Convert<unsigned char>(src.G);
			dst.R = PixelComponentConverter::Convert<unsigned char>(src.R);
			dst.A = 255;
		}
		static inline void Convert(PixelBGRA& dst, PixelRGBFloat const& src)
		{
			dst.B = PixelComponentConverter::Convert<unsigned char>(src.B);
			dst.G = PixelComponentConverter::Convert<unsigned char>(src.G);
			dst.R = PixelComponentConverter::Convert<unsigned char>(src.R);
			dst.A = 255;
		}
		static inline void Convert(PixelBGRA& dst, PixelRGBAFloat const& src)
		{
			dst.B = PixelComponentConverter::Convert<unsigned char>(src.B);
			dst.G = PixelComponentConverter::Convert<unsigned char>(src.G);
			dst.R = PixelComponentConverter::Convert<unsigned char>(src.R);
			dst.A = PixelComponentConverter::Convert<unsigned char>(src.A);
		}
		static inline void Convert(PixelBGRA& dst, PixelDepthStencil const& src)
		{
			assert(0);
		}

		/// conversion to 'PixelRGBFloat' methods
		static inline void Convert(PixelRGBFloat& dst, PixelGray const& src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<float>(src);
		}
		static inline void Convert(PixelRGBFloat& dst, PixelGrayFloat const& src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<float>(src);
		}
		static inline void Convert(PixelRGBFloat& dst, PixelBGR const& src)
		{
			dst.R = PixelComponentConverter::Convert<float>(src.R);
			dst.G = PixelComponentConverter::Convert<float>(src.G);
			dst.B = PixelComponentConverter::Convert<float>(src.B);
		}
		static inline void Convert(PixelRGBFloat& dst, PixelBGRA const& src)
		{
			dst.R = PixelComponentConverter::Convert<float>(src.R);
			dst.G = PixelComponentConverter::Convert<float>(src.G);
			dst.B = PixelComponentConverter::Convert<float>(src.B);
		}
		static inline void Convert(PixelRGBFloat& dst, PixelRGBAFloat const& src)
		{
			dst.R = PixelComponentConverter::Convert<float>(src.R);
			dst.G = PixelComponentConverter::Convert<float>(src.G);
			dst.B = PixelComponentConverter::Convert<float>(src.B);
		}
		static inline void Convert(PixelRGBFloat& dst, PixelDepthStencil const& src)
		{
			assert(0);
		}

		/// conversion to 'PixelRGBAFloat' methods
		static inline void Convert(PixelRGBAFloat& dst, PixelGray const& src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<float>(src);
			dst.A = 1.0f;
		}
		static inline void Convert(PixelRGBAFloat& dst, PixelGrayFloat const& src)
		{
			dst.B = dst.G = dst.R = PixelComponentConverter::Convert<float>(src);
			dst.A = 1.0f;
		}
		static inline void Convert(PixelRGBAFloat& dst, PixelBGR const& src)
		{
			dst.R = PixelComponentConverter::Convert<float>(src.R);
			dst.G = PixelComponentConverter::Convert<float>(src.G);
			dst.B = PixelComponentConverter::Convert<float>(src.B);
			dst.A = 1.0f;
		}
		static inline void Convert(PixelRGBAFloat& dst, PixelBGRA const& src)
		{
			dst.R = PixelComponentConverter::Convert<float>(src.R);
			dst.G = PixelComponentConverter::Convert<float>(src.G);
			dst.B = PixelComponentConverter::Convert<float>(src.B);
			dst.A = PixelComponentConverter::Convert<float>(src.A);
		}
		static inline void Convert(PixelRGBAFloat& dst, PixelRGBFloat const& src)
		{
			dst.R = PixelComponentConverter::Convert<float>(src.R);
			dst.G = PixelComponentConverter::Convert<float>(src.G);
			dst.B = PixelComponentConverter::Convert<float>(src.B);
			dst.A = 1.0f;
		}
		static inline void Convert(PixelRGBAFloat& dst, PixelDepthStencil const& src)
		{
			assert(0);
		}

		/// conversion to 'PixelDepthStencil' methods
		static inline void Convert(PixelDepthStencil& dst, PixelGrayFloat const& src)
		{
			assert(0);
		}
		static inline void Convert(PixelDepthStencil& dst, PixelBGR const& src)
		{
			assert(0);
		}
		static inline void Convert(PixelDepthStencil& dst, PixelBGRA const& src)
		{
			assert(0);
		}
		static inline void Convert(PixelDepthStencil& dst, PixelRGBFloat const& src)
		{
			assert(0);
		}
		static inline void Convert(PixelDepthStencil& dst, PixelRGBAFloat const& src)
		{
			assert(0);
		}
	};

#endif

}; // namespace chaos