namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImageProcessor;
	class ImageProcessorOutline;
	class ImageProcessorAddAlpha;
	class ImageProcessorShadow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImageProcessor : take an image as an entry and returns a new image as output
	*/

	class ImageProcessor : public Object, public JSONSerializable
	{
		CHAOS_DECLARE_OBJECT_CLASS2(ImageProcessor, Object);

	public:


		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc) const;

		/** apply processing on a whole animation (grid or not) */
		virtual std::vector<FIBITMAP*> ProcessImageFrames(std::vector<FIBITMAP*> const& src, BitmapGridAnimationInfo const& grid_anim) const;

	protected:

		/** an utility function to call functor on pixel accessor */
		template<typename FUNC>
		static FIBITMAP* DoImageProcessing(ImageDescription const& src_desc, FUNC func)
		{
			FIBITMAP* result = nullptr;

			// iterate over all PixelTypes
			meta::for_each<PixelTypes>([src_desc, func, &result](auto value) -> bool // XXX: value is an instance of identity<pixel_type>
			{
				using pixel_type = typename decltype(value)::type;
				if (src_desc.pixel_format == PixelFormat::GetPixelFormat<pixel_type>())
				{
					result = func(ImagePixelAccessor<pixel_type>(src_desc));
					return true;
				}
				return false;

			}, false);

			return result;
		}
	};

	/**
	* ImageProcessorOutline : add an outline to any image
	*/

	class ImageProcessorOutline : public ImageProcessor
	{
		CHAOS_DECLARE_OBJECT_CLASS2(ImageProcessorOutline, ImageProcessor);

	public:

		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc) const override;

		/** the processor may save its configuration into a JSON file */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;

	public:

		/** the size of the outline */
		int distance = 1;
		/** filter to check pixel to keep */
		ColorFilter color_filter;
		/** the ouline color */
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		/** the empty color */
		glm::vec4 empty_color = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	/**
	* ImageProcessorOutline : add an alpha channel according to RGB value
	*/

	class ImageProcessorAddAlpha : public ImageProcessor
	{
		CHAOS_DECLARE_OBJECT_CLASS2(ImageProcessorAddAlpha, ImageProcessor);

	public:

		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc) const override;

		/** the processor may save its configuration into a JSON file */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;



	protected:

		/** the strength of the effect */
		float strength = 1.0f;
	};

	/**
	* ImageProcessorShadow : add a shadow to an image
	*/

	class ImageProcessorShadow : public ImageProcessor
	{
		CHAOS_DECLARE_OBJECT_CLASS2(ImageProcessorShadow, ImageProcessor);

	public:

		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc) const override;

		/** the processor may save its configuration into a JSON file */
		virtual bool SerializeIntoJSON(nlohmann::json& json) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool SerializeFromJSON(nlohmann::json const& json) override;

	public:

		/** the offset of the shadow */
		glm::vec2 offset = { 5, 5 };
		/** filter to check pixel to keep */
		ColorFilter color_filter;
		/** the ouline color */
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		/** the empty color */
		glm::vec4 empty_color = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

#endif


}; // namespace chaos