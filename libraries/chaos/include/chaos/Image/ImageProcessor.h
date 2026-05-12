namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImageProcessor;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_DEFINE_LOG(ImageProcessorLog, "ImageProcessor")

	/**
	* ImageProcessor : take an image as an entry and returns a new image as output
	*/

	class CHAOS_API ImageProcessor : public Object, public JSONSerializableInterface
	{
		CHAOS_DECLARE_OBJECT_CLASS(ImageProcessor, Object);

	public:


		/** the image processing method to override */
		virtual FIBITMAP* ProcessImage(ImageDescription const& src_desc) const;

		/** apply processing on a whole animation (grid or not) */
		virtual std::vector<FIBITMAP*> ProcessImageFrames(std::vector<FIBITMAP*> const& src, BitmapGridAnimationInfo const& grid_anim) const;

	protected:

		/** an utility function to call functor on pixel accessor */
		template<typename FUNC>
		static FIBITMAP* DoImageProcessing(ImageDescription const& src_desc, FUNC const & func)
		{
			FIBITMAP* result = nullptr;

			// iterate over all PixelTypes
			meta::for_each<PixelTypes>([src_desc, func, &result](auto value) -> bool // XXX: value is an instance of identity<pixel_type>
			{
				using pixel_type = typename decltype(value)::type;
				if (src_desc.pixel_format == PixelToFormat_v<pixel_type>)
				{
					result = func(ImagePixelAccessor<pixel_type>(src_desc));
					return true;
				}
				return false;

			}, false);

			return result;
		}
	};

#endif

}; // namespace chaos