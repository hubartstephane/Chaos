namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SoundSourceLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API SoundSourceLoader : public ResourceManagerLoader<SoundSource, SoundManager>
	{
	public:

		/** constructor */
		SoundSourceLoader(SoundManager* in_sound_manager) :
			ResourceManagerLoader<SoundSource, SoundManager>(in_sound_manager)
		{
			assert(in_sound_manager != nullptr); // source cannot be loaded outside of a manager
		}

		/** load an object from JSON */
		virtual SoundSource* LoadObject(char const* name, nlohmann::json const * json) const;
		/** program loading from path */
		virtual SoundSource* LoadObject(FilePathParam const& path, char const* name = nullptr) const;

	protected:

		/** internal method to load a source */
		SoundSource* GenSourceObject(FilePathParam const& path) const;

	protected:

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const& path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(ObjectRequest request) const override;
	};

#endif

}; // namespace chaos