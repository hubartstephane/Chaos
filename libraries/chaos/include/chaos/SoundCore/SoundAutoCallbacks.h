namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SoundAutoCallbacks;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API SoundAutoCallbacks : public SoundCallbacks
	{
	public:

		/** default constructor */
		SoundAutoCallbacks() = default;
		/** assignation constructor */
		template<typename U, typename V>
		SoundAutoCallbacks(U& in_finished, V& in_removed) :
			finished_func(in_finished),
			removed_func(in_removed)
		{
		}

	protected:

		/** called whenever a sound is finished */
		virtual void OnFinished(SoundObject* object) override;
		/** called whenever an object is removed from manager */
		virtual void OnRemovedFromManager(SoundObject* object) override;

	public:

		/** the callbacks function */
		std::function<void(SoundObject*)> finished_func;
		/** the callbacks function */
		std::function<void(SoundObject*)> removed_func;
	};

#endif

}; // namespace chaos