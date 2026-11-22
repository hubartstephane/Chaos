namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SoundCategoryLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API SoundCategoryLoader : public ResourceManagerLoader<SoundCategory, SoundManager>
	{
	public:

		/** constructor */
		SoundCategoryLoader(SoundManager* in_sound_manager) :
			ResourceManagerLoader<SoundCategory, SoundManager>(in_sound_manager)
		{
			assert(in_sound_manager != nullptr); // source cannot be loaded outside of a manager
		}

		/** load an object from JSON */
		virtual SoundCategory* LoadObject(char const* name, nlohmann::json const * json) const;

	protected:

		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(ObjectRequest request) const override;
	};

#endif

}; // namespace chaos