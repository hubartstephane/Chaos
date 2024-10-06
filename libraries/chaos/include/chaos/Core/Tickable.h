namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Tickable;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ========================================================
	// TickableInterface : an object that deserve to be ticked
	// ========================================================

	class CHAOS_API TickableInterface
	{

	public:

		/** change the pause state */
		void SetPause(bool in_pause = true);
		/** get the pause state */
		bool IsPaused() const;
		/** the tick entry point */
		bool Tick(float delta_time);

	protected:

		/** the tick user function */
		virtual bool DoTick(float delta_time);
		/** whether the object should be ticked or not */
		virtual bool CanTick();
		/** called whenever object pause state has been changed */
		virtual void OnPauseStateChanged(bool in_pause);

	protected:

		/** the pause state */
		bool paused = false;
	};

	// ========================================================
	// Tickable : an object that deserve to be ticked
	// ========================================================

	class CHAOS_API Tickable : public Object, public NamedInterface, public TickableInterface
	{
		CHAOS_DECLARE_OBJECT_CLASS(Tickable, Object);
	};

#endif

}; // namespace chaos