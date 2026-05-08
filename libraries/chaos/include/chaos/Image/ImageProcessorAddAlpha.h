namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImageProcessorAddAlpha;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImageProcessorAddAlpha : add an alpha channel according to RGB value
	*/

	class CHAOS_API ImageProcessorAddAlpha : public ImageProcessor
	{
		CHAOS_DECLARE_OBJECT_CLASS(ImageProcessorAddAlpha, ImageProcessor);

	public:

		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc) const override;

		/** the processor may save its configuration into a JSON file */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool SerializeFromJSON(JSONReadConfiguration config) override;

	protected:

		/** the strength of the effect */
		float strength = 1.0f;
	};

#endif


}; // namespace chaos