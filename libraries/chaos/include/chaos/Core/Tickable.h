#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class Tickable;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else 

namespace chaos
{

	// ========================================================
	// Tickable : an object that deserve to be ticked
	// ========================================================	

	class Tickable : public Object, public NamedObject
	{

		CHAOS_DECLARE_OBJECT_CLASS2(Tickable, Object);

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

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


