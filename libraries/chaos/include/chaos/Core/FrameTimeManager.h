namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class FrameTimeManager;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* FrameTimeManager: an object to handle frame timing
	*/

	class CHAOS_API FrameTimeManager : public Singleton<FrameTimeManager>
	{
	public:

		/** update the time of current frame */
		void SetCurrentFrameTime(double in_frame_time);
		/** get the current frame time */
		double GetFrameTime() const;

		/** get the duration of last frame */
		double GetCurrentFrameDuration() const;

	protected:

		/** the current frame time */
		double current_frame_time = -1.0;
		/** the last frame time */
		double previous_frame_time = -1.0;
	};

#endif

}; // namespace chaos