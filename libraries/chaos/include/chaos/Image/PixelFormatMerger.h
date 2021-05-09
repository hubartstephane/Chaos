namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class PixelFormatMergeParams;
	class PixelFormatMerger;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* PixelFormatMergeParams
	*/

	class PixelFormatMergeParams
	{

	public:

		/** if valid, the resulting pixel format should be set to this */
		PixelFormat pixel_format;
		/** if false the pixel_format cannot be 'upgraded' with incomming format */
		bool upgrade_pixel_format = true;
		/** if set to false, all luminances formats will become RGB */
		bool accept_luminance = true;
		/** if set to false, all float formats will become unsigned char */
		bool accept_float = true;
	};

	bool LoadFromJSON(nlohmann::json const& json_entry, PixelFormatMergeParams& dst);

	bool SaveIntoJSON(nlohmann::json& json_entry, PixelFormatMergeParams const& src);

	/**
	* PixelFormatMerger : utility function to find a common PixelFormat for multiple images
	*/

	class PixelFormatMerger
	{

	public:

		/** constructor */
		PixelFormatMerger(PixelFormatMergeParams const& in_params = {});

		/** reset the merger */
		void Reset(PixelFormatMergeParams const& in_params = {});
		/** the merge method */
		void Merge(PixelFormat src);
		/** get the result */
		PixelFormat GetResult() const { return result; }
		/** check whether the result is available */
		bool IsResultAvailable() const { return result_is_available; }
		/** returns whether all incomming format match the result */
		bool AreIncommingFormatIdentical() const { return identical_incomming_format; }

	protected:

		/** the result */
		PixelFormat result;
		/** whether a valid result is available */
		bool result_is_available = false;
		/** the parameter for the merge */
		PixelFormatMergeParams params;
		/** true whether all incomming format are identical (no conversion required) */
		bool identical_incomming_format = true;
	};


#endif

}; // namespace chaos