namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class PixelComponentType;

	enum class PixelFormat;

	class PixelDescription;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * PixelComponentType: the type of each component of each pixels
	 */

	enum class PixelComponentType : int
	{
		Unknown = 0,
		UnsignedChar = 1,
		Float = 2,
		DepthStencil = 3
	};

	CHAOS_DECLARE_ENUM_METHOD(PixelComponentType, CHAOS_API);

	/**
	 * PixelFormat: supported kind of pixels
	 */

	enum class PixelFormat : int
	{
		Unknown,
		BGR,
		BGRA,
		Gray,
		RGBFloat,
		RGBAFloat,
		GrayFloat,
		DepthStencil
	};

	CHAOS_DECLARE_ENUM_METHOD(PixelFormat, CHAOS_API);

	CHAOS_GENERATE_VALUE_MAPPING_DECLARATION(PixelToFormat, typename);
	CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(PixelToFormat, PixelBGR, PixelFormat, PixelFormat::BGR);
	CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(PixelToFormat, PixelBGRA, PixelFormat, PixelFormat::BGRA);
	CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(PixelToFormat, PixelGray, PixelFormat, PixelFormat::Gray);
	CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(PixelToFormat, PixelRGBFloat, PixelFormat, PixelFormat::RGBFloat);
	CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(PixelToFormat, PixelRGBAFloat, PixelFormat, PixelFormat::RGBAFloat);
	CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(PixelToFormat, PixelGrayFloat, PixelFormat, PixelFormat::GrayFloat);
	CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(PixelToFormat, PixelDepthStencil, PixelFormat, PixelFormat::DepthStencil);

	CHAOS_API bool IsValidFormat(PixelFormat pixel_format);

	CHAOS_API bool IsColorFormat(PixelFormat pixel_format);

	CHAOS_API bool IsLuminanceFormat(PixelFormat pixel_format);

	CHAOS_API bool IsDepthStencilFormat(PixelFormat pixel_format);

	CHAOS_API int GetPixelSize(PixelFormat pixel_format);

	CHAOS_API PixelDescription GetPixelDescription(PixelFormat pixel_format);

	/**
	* PixelDescription: describe pixels
	*/

	class CHAOS_API PixelDescription
	{

	public:

		/** constructor */
		PixelDescription() = default;

		PixelDescription(PixelDescription const& other) = default;

		PixelDescription(PixelComponentType in_component_type, int in_component_count) :
			component_type(in_component_type),
			component_count(in_component_count) {};

	public:

		/** the type of the components */
		PixelComponentType component_type = PixelComponentType::Unknown;
		/** the number of components for each pixels */
		int component_count = 0;
	};

	CHAOS_API PixelFormat GetPixelFormat(PixelDescription const & pixel_description);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, PixelDescription& dst);

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, PixelDescription const& src);

#endif

}; // namespace chaos