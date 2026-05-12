namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class PixelFormatMergeParams;
	class PixelFormatMerger;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* PixelFormatMergeParams
	*/

	class CHAOS_API PixelFormatMergeParams
	{

	public:

		/** if valid, the resulting pixel format should be set to this */
		PixelFormat pixel_format = PixelFormat::Unknown;
		/** if false the pixel_format cannot be 'upgraded' with incomming format */
		bool upgrade_pixel_format = true;
		/** if set to false, all luminances formats will become RGB */
		bool accept_luminance = true;
		/** if set to false, all float formats will become unsigned char */
		bool accept_float = true;
	};

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, PixelFormatMergeParams& dst);

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json * json, PixelFormatMergeParams const& src);

	/**
	* PixelFormatMerger : utility function to find a common PixelFormat for multiple images
	*/

	class CHAOS_API PixelFormatMerger
	{

	public:

		/** constructor */
		PixelFormatMerger(PixelFormatMergeParams const& in_params = {});

		/** reset the merger */
		void Reset(PixelFormatMergeParams const& in_params = {});
		/** the merge method */
		void Merge(PixelFormat in_pixel_format);
		/** get the result */
		PixelFormat GetResult() const;

	protected:

		/** apply wanted parameters to a pixel format */
		PixelFormat ApplyParametersToFormat(PixelFormat in_pixel_format) const;

	protected:

		/** the parameter for the merge */
		PixelFormatMergeParams params;
		/** the result */
		PixelFormat pixel_format;
		/** an intermdiate description for working easier */
		PixelDescription pixel_description;
	};


#endif

}; // namespace chaos