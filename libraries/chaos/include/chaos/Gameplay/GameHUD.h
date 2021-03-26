#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GameHUD;
	class MainMenuHUD;
	class PauseMenuHUD;
	class GameOverHUD;
	class PlayingHUD;

}; // namespace chaos

#else

namespace chaos
{

	// =============================================
	// GameHUD
	// =============================================

	class GameHUD : public GPURenderable
	{
		CHAOS_GAMEPLAY_ALLFRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS2(GameHUD, GPURenderable);

	public:

		/** getters on game */
		AutoCastable<Game> GetGame() { return game; }
		/** getters on game */
		AutoConstCastable<Game> GetGame() const { return game; }

		/** getters on game instance */
		AutoCastable<GameInstance> GetGameInstance();
		/** getters on game instance */
		AutoConstCastable<GameInstance> GetGameInstance() const;

		/** get currently played level */
		AutoCastable<Level> GetLevel();
		/** get currently played level */
		AutoConstCastable<Level> GetLevel() const;

		/** get currently played level */
		AutoCastable<LevelInstance> GetLevelInstance();
		/** get currently played level */
		AutoConstCastable<LevelInstance> GetLevelInstance() const;

		/** get the player */
		AutoCastable<Player> GetPlayer(size_t player_index);
		/** get the player */
		AutoConstCastable<Player> GetPlayer(size_t player_index) const;
		/** get the number of player */
		size_t GetPlayerCount() const;

		/** find a component by its class */
		template<typename T>
		T * FindComponentByClass()
		{
			for (auto it = components.begin(); it != components.end(); ++it)
			{
				T * result = auto_cast(it->second.get());
				if (result != nullptr)
					return result;
			}
			return nullptr;
		}
		/** find a component by its class */
		template<typename T>
		T const * FindComponentByClass() const
		{
			for (auto it = components.begin(); it != components.end(); ++it)
			{
				T * result = auto_cast(it->second.get());
				if (result != nullptr)
					return result;
			}
			return nullptr;
		}

		/** initialization of the HUD from the game values */
		virtual bool Initialize(Game* in_game);

		/** insert a component inside the HUD */
		template<typename COMPONENT_TYPE, typename ...PARAMS>
		void RegisterComponent(TagType key, COMPONENT_TYPE * component, PARAMS... params)
		{
			// XXX : why is this method a template ??
			//       a COMPONENT initialization requires the HUD for some resources
			//       so you cannot build the COMPONENT before the insertion
			//       you need to do both at the same time, and you require various kind of parameters
			//       COMPONENT initialization (OnInsertedInHUD(...)) cannot be virtual because we don't know the parameters for the construction
			//       that's why we use a template function
			assert(component != nullptr);
			assert(component->hud == nullptr);
			// remove previous component with the key
			UnregisterComponent(key);
			// register new component for that key
			components.insert(std:: make_pair(key, component));
			component->SetHUD(this);
			InitializeComponentFromConfiguration(key, component); // this will override the component position from JSON file
			component->OnInsertedInHUD(params...);			
		}
		/** remove a component from the HUD */
		void UnregisterComponent(TagType key);
		/** remove a component from the HUD */
		void UnregisterComponent(GameHUDComponent * in_component);

		/** get the component from its ID */
		GameHUDComponent * FindComponent(TagType key);
		/** get the component from its ID */
		GameHUDComponent const * FindComponent(TagType key) const;

		/** insert some particles inside the HUD */
		void RegisterParticles(TagType key, ParticleAllocationBase * allocation, bool remove_previous = true);
		/** remove some particles from the HUD */
		void UnregisterParticles(TagType key);
		/** clear all particles from the HUD */
		void Clear();

		/** get the particle allocation from its ID */
		ParticleAllocationBase * FindParticleAllocation(TagType key);
		/** get the particle allocation from its ID */
		ParticleAllocationBase const * FindParticleAllocation(TagType key) const;

		/** get the particle manager */
		ParticleManager * GetParticleManager() { return particle_manager.get(); }
		/** get the particle manager */
		ParticleManager const * GetParticleManager() const { return particle_manager.get(); }

		/** get particle tools */
		GameParticleCreator GetGameParticleCreator();

	protected:

		/** initialization method */
		virtual bool CreateInternalData(ParticleManager * in_particle_manager, ParticleTextGenerator::Generator * in_particle_text_generator, BitmapAtlas::TextureArrayAtlas * in_texture_atlas);
		/** create the layers */
		virtual int CreateHUDLayers();
		/** create the particles */
		virtual bool FillHUDContent();
		/** try to find some information in JSON for configuring the component */
		void InitializeComponentFromConfiguration(TagType key, GameHUDComponent * component);

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual int DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params) override;

	protected:

		/** the allocations */
		std::map<TagType, shared_ptr<GameHUDComponent>> components;
		/** the allocations */
		std::multimap<TagType, shared_ptr<ParticleAllocationBase>> particle_allocations;
		/** the particle manager */
		shared_ptr<ParticleManager> particle_manager;
		/** indicates whether THIS is responsible or not of rendering/ticking the particle manager */
		bool external_manager = false;

		/** the game owning the HUD */
		class Game * game = nullptr;
	};

	// =============================================
	// MainMenuHUD
	// =============================================

	class MainMenuHUD : public GameHUD
	{
		CHAOS_DECLARE_OBJECT_CLASS2(MainMenuHUD, GameHUD);

	protected:

		/** override */
		virtual bool FillHUDContent() override;
	};

	// =============================================
	// PauseMenuHUD
	// =============================================

	class PauseMenuHUD : public GameHUD
	{

		CHAOS_DECLARE_OBJECT_CLASS2(PauseMenuHUD, GameHUD);

	protected:

		/** override */
		virtual bool FillHUDContent() override;
	};

	// =============================================
	// GameOverHUD
	// =============================================

	class GameOverHUD : public GameHUD
	{

		CHAOS_DECLARE_OBJECT_CLASS2(GameOverHUD, GameHUD);

	protected:

		/** override */
		virtual bool FillHUDContent() override;
	};

	// =============================================
	// PlayingHUD
	// =============================================

	class PlayingHUD : public GameHUD
	{

		CHAOS_DECLARE_OBJECT_CLASS2(PlayingHUD, GameHUD);

	protected:

		/** override */
		virtual bool FillHUDContent() override;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION