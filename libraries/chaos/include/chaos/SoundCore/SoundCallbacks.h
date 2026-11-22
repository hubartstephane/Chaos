namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SoundCallbacks;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API SoundCallbacks : public Object
	{
		friend class SoundObject;

	protected:

		/** called whenever an object is finished */
		virtual void OnFinished(SoundObject* object);
		/** called whenever an object is removed from manager */
		virtual void OnRemovedFromManager(SoundObject* object);
	};

#endif

}; // namespace chaos