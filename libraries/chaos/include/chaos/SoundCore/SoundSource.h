namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SoundSource;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API SoundSource : public SoundObject, public FileResourceInterface
	{
		friend class Sound;
		friend class SoundManager;
		friend class SoundSourceLoader;

	public:

		/** generating and playing a sound */
		Sound* Play(PlaySoundDesc const& play_desc, SoundCallbacks* in_callbacks = nullptr);
		/** set the categories */
		bool SetDefaultCategories(std::vector<SoundCategory*> const& categories);

	protected:

		/** override */
		virtual void DoUpdateEffectiveVolume(float effective_volume);
		/** override */
		virtual void DoUpdateEffectivePause(bool effective_pause);

		/** unbind from manager */
		virtual void OnRemovedFromManager() override;
		/** remove element from manager list and detach it */
		virtual void RemoveFromManager() override;
		/** the sound generation method */
		virtual Sound* GenerateSound();

		/** loading from a JSON object */
		virtual bool InitializeFromJSON(nlohmann::json const * json) override;

	protected:

		/** the irrklang source */
		shared_ptr<irrklang::ISoundSource> irrklang_source;
		/** the default category */
		std::vector<SoundCategory*> default_categories;
	};

#endif

}; // namespace chaos