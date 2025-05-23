namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class BitmapGridAnimationInfo;
	class ImageAnimationDescription;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* BitmapGridAnimationInfo : some bitmaps represent a uniform grid of individual animation frames
	*/

	class CHAOS_API BitmapGridAnimationInfo
	{
	public:

		/** the size of the grid */
		glm::ivec2 grid_size = { 0, 0 };
		/** the last images that are not filled */
		int skip_lasts = 0;

	public:

		/** return the number of frames of the animation */
		int GetFrameCount() const;

		/** parsing the the name to extract the grid numbers */
		static bool ParseFromName(char const* name, BitmapGridAnimationInfo& result, std::string* name_result = nullptr);

	protected:

		/** utility method */
		static bool ParseFromNameReadGridX(char const* name, int i, BitmapGridAnimationInfo& result, std::string* name_result = nullptr);
		/** utility method */
		static bool ParseFromNameReadGridY(char const* name, int i, BitmapGridAnimationInfo& result, std::string* name_result = nullptr);
		/** utility method */
		static bool ParseFromNameReadGridSkip(char const* name, int i, BitmapGridAnimationInfo& result, std::string* name_result = nullptr);
		/** utility method */
		static bool ParseDigitReverse(char const* str, int& start);
	};

	/**
	* ImageAnimationDescription : data to work with animation
	*/

	class CHAOS_API ImageAnimationDescription
	{
	public:

		/** returns true whether the animation is of type frame (i.e GIF) */
		bool IsFrameAnimation() const;
		/** returns true whether the animation is of type grid */
		bool IsGridAnimation() const;
		/** get the duration of a frame in seconds */
		float GetFrameDuration() const;
		/** get the duration of the whole animation in seconds */
		float GetAnimationDuration() const;
		/** return the number of frames of the animation */
		int GetFrameCount() const;
		/** returns the default wrap mode when accessing this animation */
		WrapMode GetDefaultWrapMode() const { return default_wrap_mode; }

	public:

		/** if the animation is stored inside a grid */
		BitmapGridAnimationInfo grid_data;
		/** number of child frame (directly following the AtlasBitmapInfo */
		int child_frame_count = 0;
		/** the duration of a frame in seconds */
		float frame_duration = -1.0f;
		/** the duration of the whole animation */
		float anim_duration = -1.0f;
		/** the default wrap mode */
		WrapMode default_wrap_mode = WrapMode::NONE; // let the code decide in a single location (see  AtlasBitmapInfo::GetEffectiveRequestWrapMode(...))
	};

	/**
	* Some JSON utility functions
	*/

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json * json, BitmapGridAnimationInfo const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, BitmapGridAnimationInfo& dst);

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json * json, ImageAnimationDescription const& src);

	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, ImageAnimationDescription& dst);


#endif

}; // namespace chaos