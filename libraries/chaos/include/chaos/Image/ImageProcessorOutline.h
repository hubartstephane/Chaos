namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImageProcessorOutline;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImageProcessorOutline : add an outline to any image
	*/

	class CHAOS_API ImageProcessorOutline : public ImageProcessor
	{
		CHAOS_DECLARE_OBJECT_CLASS(ImageProcessorOutline, ImageProcessor);

	public:

		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc) const override;

		/** the processor may save its configuration into a JSON file */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool SerializeFromJSON(JSONReadConfiguration config) override;

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

#endif


}; // namespace chaos