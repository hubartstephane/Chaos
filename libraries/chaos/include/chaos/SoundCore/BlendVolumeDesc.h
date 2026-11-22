namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class SoundBlendType;

	class BlendVolumeDesc;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class SoundBlendType : int
	{
		None = 0,
		BlendIn = 1,
		BlendOut = 2
	};

	class CHAOS_API BlendVolumeDesc
	{
	public:

		/** helper function to create a BLEND_IN effect */
		static BlendVolumeDesc BlendIn(float blend_time);
		/** helper function to create a BLEND_OUTeffect */
		static BlendVolumeDesc BlendOut(float blend_time, bool pause_at_end = false, bool kill_at_end = false, bool kill_when_paused = false);

	public:

		/** the kind of blending */
		SoundBlendType blend_type = SoundBlendType::None;
		/** the time to blend from [0 to 1] or [1 to 0] => if current blend value is between, the time is renormalized */
		float blend_time = 1.0f;
		/** whether the object should be paused at the end of blend */
		bool  pause_at_end = false;
		/** whether the object should be killed at the end of blend */
		bool  kill_at_end = false;
		/** whether the object should be killed when paused */
		bool  kill_when_paused = false;
		/** some callbacks */
		shared_ptr<SoundCallbacks> callbacks;
	};

#endif

}; // namespace chaos