namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SoundCategory;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API SoundCategory : public SoundObject
	{
		friend class SoundSource;
		friend class SoundCategoryLoader;

	protected:

		/** override */
		virtual void DoUpdateEffectiveVolume(float effective_volume);
		/** override */
		virtual void DoUpdateEffectivePause(bool effective_pause);

		/** unbind from manager */
		virtual void OnRemovedFromManager() override;
		/** remove element from manager list and detach it */
		virtual void RemoveFromManager() override;
	};

#endif

}; // namespace chaos