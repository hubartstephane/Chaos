namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	using PixelGray = unsigned char; // a grayscale pixel
	using PixelGrayFloat = float; // a grayscale pixel (float data)

	class PixelBGR;
	class PixelBGRA;
	class PixelRGBFloat;
	class PixelRGBAFloat;
	class PixelDepthStencil;

	/** PixelTypes : the list of all supported pixel classes (XXX : order matters) */
	using PixelTypes = boost::mpl::vector<PixelGray, PixelBGR, PixelBGRA, PixelGrayFloat, PixelRGBFloat, PixelRGBAFloat, PixelDepthStencil>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// FreeImage produces
	//
	//   - BGR(A) pixels for UNSIGNED CHAR images
	//   - RGB(A) pixels for FLOAT images
	//
	// The difference come from endianess. That have no signification for FLOAT but for 32bits encoded values it has !
	//

	/**
	* PixelBGR : a structure that helps manipulating 24 bit pixel
	**/

	class CHAOS_API PixelBGR // FreeImage produce BGR pixels by default (for unsigned char)
	{
	public:

		/** constructor */
		PixelBGR() = default;
		/** copy constructor */
		PixelBGR(PixelBGR const& src) = default;
		/** conversion */
		PixelBGR(unsigned int color) { operator = (color); }

		/** assignation */
		PixelBGR& operator = (unsigned int color)
		{
			B = (color >> 0) & 0xFF;
			G = (color >> 8) & 0xFF;
			R = (color >> 16) & 0xFF;
			return *this;
		}

		unsigned char B = 0;
		unsigned char G = 0;
		unsigned char R = 0;
	};

	/**
	* PixelBGRA : a structure that helps manipulating 32 bit pixel
	**/

	class CHAOS_API PixelBGRA // FreeImage produce BGRA pixels by default (for unsigned char)
	{
	public:

		/** constructor */
		PixelBGRA() = default;
		/** copy constructor */
		PixelBGRA(PixelBGRA const& src) = default;
		/** conversion */
		PixelBGRA(unsigned int color) { operator = (color); }

		/** assignation */
		PixelBGRA& operator = (unsigned int color)
		{
			B = (color >> 0) & 0xFF;
			G = (color >> 8) & 0xFF;
			R = (color >> 16) & 0xFF;
			A = (color >> 24) & 0xFF;
			return *this;
		}

		unsigned char B = 0;
		unsigned char G = 0;
		unsigned char R = 0;
		unsigned char A = 0;
	};

	/**
	* PixelRGBFloat : a structure that helps manipulating 3 x float pixels
	**/

	class CHAOS_API PixelRGBFloat // FreeImage produce RGB pixels by default (for float) !!!
	{
	public:

		/** constructor */
		PixelRGBFloat() = default;
		/** copy constructor */
		PixelRGBFloat(PixelRGBFloat const& src) = default;
		/** conversion */
		PixelRGBFloat(glm::vec3 const& color) { operator = (color); }

		/** assignation */
		PixelRGBFloat& operator = (glm::vec3 const& color)
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

		float R = 0.0f;
		float G = 0.0f;
		float B = 0.0f;
	};

	/**
	* PixelRGBAFloat : a structure that helps manipulating 4 x float pixels
	**/

	class CHAOS_API PixelRGBAFloat // FreeImage produce RGBA pixels by default (for float) !!!
	{
	public:

		/** constructor */
		PixelRGBAFloat() = default;
		/** copy constructor */
		PixelRGBAFloat(PixelRGBAFloat const& src) = default;
		/** conversion */
		PixelRGBAFloat(glm::vec4 const& color) { operator = (color); }

		/** automatic conversion */
		PixelRGBAFloat& operator = (glm::vec4 const& color)
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

		float R = 0.0f;
		float G = 0.0f;
		float B = 0.0f;
		float A = 0.0f;
	};

	/**
	* PixelDepthStencil : a structure that helps depth24+stencil8 pixels
	**/

	class CHAOS_API PixelDepthStencil // FreeImage produce RGBA pixels by default (for float) !!!
	{
	public:

		/** constructor */
		PixelDepthStencil() = default;
		/** copy constructor */
		PixelDepthStencil(PixelDepthStencil const& src) = default;

		unsigned char special1 = 0;
		unsigned char special2 = 0;
		unsigned char special3 = 0;
		unsigned char special4 = 0;
	};

#endif

}; // namespace chaos