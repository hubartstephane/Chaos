namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SoundManager;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_DEFINE_LOG(SoundLog, "Sound")

	class CHAOS_API SoundManager : public ResourceManager, public ConfigurationUserInterface
	{
		friend class SoundObject;
		friend class SoundSource;
		friend class SoundCategory;
		friend class Sound;
		friend class SoundSourceLoader;

	public:

		/** getter on the irrklang engine */
		irrklang::ISoundEngine* GetIrrklangEngine();

		/** public method to tick the manager */
		void Tick(float delta_time);

		/** find a source by its name */
		SoundSource* FindSource(ObjectRequest request);
		/** find a source by its name */
		SoundSource const* FindSource(ObjectRequest request) const;

		/** find a simple source by its path */
		SoundSource* FindSourceByPath(FilePathParam const& path);
		/** find a simple source by its path */
		SoundSource const* FindSourceByPath(FilePathParam const& path) const;

		/** find a category by its name */
		SoundCategory* FindCategory(ObjectRequest request);
		/** find a category by its name */
		SoundCategory const* FindCategory(ObjectRequest request) const;

		/** find a sound by its name */
		Sound* FindSound(ObjectRequest request);
		/** find a sound by its name */
		Sound const* FindSound(ObjectRequest request) const;

		/** add a category inside the manager */
		SoundCategory* AddCategory(char const* name);
		/** load and add a simple source inside the manager */
		SoundSource* AddSource(FilePathParam const& path, char const* name = nullptr);

		/** update the listener position */
		bool SetListenerPosition(glm::vec3 const& position, glm::vec3 const& velocity = { 0.0f, 0.0f, 0.0f });

		/** getters on sound */
		size_t GetSoundCount() const;
		Sound* GetSound(size_t index);
		Sound const* GetSound(size_t index) const;

		/** getters on category */
		size_t GetCategoryCount() const;
		SoundCategory* GetCategory(size_t index);
		SoundCategory const* GetCategory(size_t index) const;

		/** getters on sources */
		size_t GetSourceCount() const;
		SoundSource* GetSource(size_t index);
		SoundSource const* GetSource(size_t index) const;

		/** get the current listener position */
		glm::vec3 GetListenerPosition() const;
		/** get the current listener velocity */
		glm::vec3 GetListenerVelocity() const;

	protected:

		/** override */
		virtual bool DoStartManager() override;
		/** override */
		virtual bool DoStopManager() override;
		/** override */
		virtual bool OnInitialize(JSONReadConfiguration config) override;
		/** override */
		virtual bool OnConfigurationChanged(JSONReadConfiguration config) override;

		/** remove a category from the list */
		void RemoveCategory(SoundCategory* category);
		/** remove a sound from the list */
		void RemoveSound(Sound* sound);
		/** remove a sound source from the list */
		void RemoveSource(SoundSource* source);

		/** remove a category from the list */
		void RemoveCategoryByIndex(size_t index);
		/** remove a sound from the list */
		void RemoveSoundByIndex(size_t index);
		/** remove a sound source from the list */
		void RemoveSourceByIndex(size_t index);

		/** called whenever an object is being removed */
		static void OnObjectRemovedFromManager(SoundObject* object);

		/** destroy all sounds in a category */
		void DestroyAllSoundPerCategory(SoundCategory* category);
		/** destroy all sounds with a given source */
		void DestroyAllSoundPerSource(SoundSource* source);
		/** remove the category of all sources using given argument */
		void UpdateAllSourcesPerCategory(SoundCategory* category);

		/** internal tick list of objects */
		template<typename T, typename REMOVE_FUNC>
		void DoTickObjects(float delta_time, T& vector, REMOVE_FUNC remove_func)
		{
			T objects = vector; // XXX: work on a copy because due to callbacks, the input vector is volatile

			size_t count = objects.size();
			for (size_t i = 0; i < count; ++i)
			{
				auto object = objects[i]; // copy the intrusive_ptr to prevent the destruction
				if (object == nullptr || !object->IsAttachedToManager())
					continue;

				// test whether object was already finished before ticking
				bool finished = object->IsFinished();
				bool paused = object->IsEffectivePaused();

				// call tick if required
				if (!finished && !paused)
				{
					object->TickObject(delta_time);
					finished = object->IsAttachedToManager() && object->UpdateFinishedState(); // XXX : if not attached to manager, do not call OnObjectFinished(...) and there is no need to remove it from manager
				}
				// remove the object if needed
				if (finished)
				{
					object->OnObjectFinished();
					if (object->IsAttachedToManager())
						(this->*remove_func)(object.get());
				}
			}
		}

		/** test whether a category with given name could be inserted in the manager */
		bool CanAddCategory(ObjectRequest request) const;
		/** test whether a source with given name could be inserted in the manager */
		bool CanAddSource(ObjectRequest request) const;
		/** test whether a sound with given name could be inserted in the manager */
		bool CanAddSound(ObjectRequest request) const;

		/** update all sounds pause per category */
		void UpdateAllSoundPausePerCategory(SoundCategory* category);
		/** update all sounds volume per category */
		void UpdateAllSoundVolumePerCategory(SoundCategory* category);
		/** update all sounds pause per source */
		void UpdateAllSoundPausePerSource(SoundSource* source);
		/** update all sounds volume per source */
		void UpdateAllSoundVolumePerSource(SoundSource* source);

		/** load the categories from configuration */
		bool LoadCategories(nlohmann::json const * config);
		/** load the sources from configuration */
		bool LoadSources(nlohmann::json const * config);

	protected:

		/** all detected devices */
		shared_ptr<irrklang::ISoundDeviceList> irrklang_devices;
		/** the irrklank engine */
		shared_ptr<irrklang::ISoundEngine> irrklang_engine;

		/** the sources */
		std::vector<shared_ptr<SoundSource>> sources;
		/** the sounds */
		std::vector<shared_ptr<Sound>> sounds;
		/** the categories */
		std::vector<shared_ptr<SoundCategory>> categories;

		/** the listener transform */
		glm::mat4 listener_transform = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
		/** the listener velocity */
		glm::vec3 listener_velocity = { 0.0f, 0.0f, 0.0f };
	};

#endif

}; // namespace chaos