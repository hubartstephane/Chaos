#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GameHUDComponent;
	class GameHUDMeshComponent;
	class GameHUDTextComponent;

	template<typename T>
	class GameHUDCacheValueComponent;

	class GameHUDNotificationComponent;
	class GameHUDScoreComponent;
	class GameHUDFramerateComponent;
	class GameHUDTimeoutComponent;
	class GameHUDTimeoutComponent;
	class GameHUDLifeComponent;
	class GameHUDLevelTitleComponent;
	class GameHUDFreeCameraComponent;

}; // namespace chaos

#else

namespace chaos
{

	// ====================================================================
	// GameHUDComponent
	// ====================================================================

	class GameHUDComponent : public GPURenderable
	{
		CHAOS_GAMEPLAY_ALLFRIENDS;

		friend class GameHUD;
		friend class GameHUDTimedComponent;

	public:

		/** getters on game */
		AutoCastable<Game> GetGame();
		/** getters on game */
		AutoConstCastable<Game> GetGame() const;

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

	protected:

		/** returns the coordinate of the view corner corresponding to the given hotpoint */
		static glm::vec2 GetCanvasBoxCorner(box2 const & canvas_box, Hotpoint hotpoint);
		/** called whenever the hud is beeing inserted into the hud */
		virtual void OnInsertedInHUD();
		/** called whenever the hud is beeing removed into the hud */
		virtual void OnRemovedFromHUD();
		/** initialization method from JSON */
		virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);
		/** set the HUD */
		virtual void SetHUD(GameHUD * in_hud);

	protected:

		/** the current hud */
		GameHUD * hud = nullptr;
	};

	// ====================================================================
	// GameHUDMeshComponent
	// ====================================================================

	/** a HUD component with a mesh */
	class GameHUDMeshComponent : public GameHUDComponent
	{
	protected:

		/** show or hide the component */
		virtual void ShowComponent(bool in_show);
		/** override */
		virtual void OnRemovedFromHUD() override;
		/** override */
		virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params) override;

	protected:

		/** the mesh for this component */
		shared_ptr<GPUDynamicMesh> mesh;
	};


	// ====================================================================
	// GameHUDTextComponent
	// ====================================================================

	class GameHUDTextComponent : public GameHUDMeshComponent
	{
		friend class GameHUD;

	public:

		/** constructor */
		GameHUDTextComponent(char const* in_text = "", ParticleTextGenerator::GeneratorParams const& in_params = {});

	protected:

		/** tweak the text generation parameters */
		virtual void TweakTextGeneratorParams(ParticleTextGenerator::GeneratorParams & final_params) const;
		/** create the text */
		virtual void SetText(char const * in_text);
		/** update the mesh according to internal data */
		virtual void UpdateTextMesh();
		/** override */
		virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path) override;
		/** override */
		virtual void OnInsertedInHUD() override;

	protected:

		/** the text or the format to display */
		std::string text;
		/** the placement and aspect of the text */
		ParticleTextGenerator::GeneratorParams generator_params;
	};

	// ====================================================================
	// GameHUDCacheValueComponent
	// ====================================================================

	template<typename T>
	class GameHUDCacheValueComponent : public GameHUDTextComponent
	{
		using type = T;

	protected:

		/** constructor */
		using GameHUDTextComponent::GameHUDTextComponent;

		/** gets the value */
		virtual type QueryValue() const { return type(); }
		/** override */
		virtual void OnInsertedInHUD()
		{
			cached_value = QueryValue();
			GameHUDTextComponent::OnInsertedInHUD();
		}
		/** override */
		virtual bool DoUpdateGPUResources(GPURenderer* renderer) override
		{
			type new_value = QueryValue();
			if (cached_value != new_value)
			{
				cached_value = new_value;
				UpdateTextMesh();
			}
			return true;
		}
		/** updates the text according to the cached_value and the format */
		virtual void UpdateTextMesh() override
		{
			SetText(StringTools::Printf(text.c_str(), cached_value).c_str());
		}

	protected:

		/** the cached value */
		type cached_value;
	};

	// ====================================================================
	// GameHUDNotificationComponent
	// ====================================================================

	class GameHUDNotificationComponent : public GameHUDTextComponent
	{
		friend class GameHUD;

	public:

		/** constructor */
		GameHUDNotificationComponent();
		/** constructor */
		GameHUDNotificationComponent(ParticleTextGenerator::GeneratorParams const & in_params);

		/** show/initialize the notification */
		void ShowNotification(char const * in_message, float in_lifetime);
		/** hide the notification */
		void HideNotification();

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** the life of the component */
		float lifetime = -1.0f;
		/** the current time */
		float current_time = 0.0f;
	};














	// ====================================================================
	// GameHUDScoreComponent
	// ====================================================================

	class GameHUDScoreComponent : public GameHUDCacheValueComponent<int>
	{
	public:

		/** constructor */
		GameHUDScoreComponent(char const * in_text = "Score: %d");
		/** constructor */
		GameHUDScoreComponent(char const* in_text, ParticleTextGenerator::GeneratorParams const & in_params):
			GameHUDCacheValueComponent<int>(in_text, in_params) {}

	protected:

		/** override */
		virtual int QueryValue() const override;
	};

	// ====================================================================
	// GameHUDFramerateComponent
	// ====================================================================

	class GameHUDFramerateComponent : public  GameHUDCacheValueComponent<float>
	{
	public:

		/** constructor */
		GameHUDFramerateComponent(char const* in_text = "%02.01f FPS");
		/** constructor */
		GameHUDFramerateComponent(char const* in_text, ParticleTextGenerator::GeneratorParams const & in_params):
			GameHUDCacheValueComponent<float>(in_text, in_params) {}

	protected:

		/** query value */
		virtual float QueryValue() const override;
	};

	// ====================================================================
	// GameHUDTimeoutComponent
	// ====================================================================

	class GameHUDTimeoutComponent : public GameHUDCacheValueComponent<float>
	{
	public:

		/** constructor */
		GameHUDTimeoutComponent(char const * in_text = "%02.01f");
		/** constructor */
		GameHUDTimeoutComponent(char const* in_text, ParticleTextGenerator::GeneratorParams const & in_params) :
			GameHUDCacheValueComponent<float>(in_text, in_params) {}

	protected:

		/** override */
		virtual float QueryValue() const override;
		/** override */
		virtual void UpdateTextMesh() override;
		/** override */
		virtual void TweakTextGeneratorParams(ParticleTextGenerator::GeneratorParams & final_params) const override;
	};


	// ====================================================================
	// GameHUDLifeComponent
	// ====================================================================

	class GameHUDLifeComponent : public GameHUDMeshComponent
	{
	public:

		/** constructor */
		GameHUDLifeComponent() = default;

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** update all particles (count, alpha) */
		void UpdateLifeParticles(float delta_time);
		/** returns the number of life */
		virtual int GetLifeCount() const;
		/** tick heart */
		void TickHeartBeat(float delta_time);
		/** override */
		virtual bool InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path) override;


	public:

		/** an alternate way to have life count */
		std::function<int()> get_life_count_func;

	protected:

		/** caching the current life count */
		int cached_value = -1;

		/** the hotpoint of the first particle */
		Hotpoint hotpoint = Hotpoint::BOTTOM_LEFT;
		/** the position of the very first particle */
		glm::vec2 position = glm::vec2(0.0f, 0.0f);
		/** the particle size of the particle (special meaning, see .cpp) */
		glm::vec2 particle_size = glm::vec2(0.0f, 0.0f);
		/** the offset to apply between each particles */
		glm::vec2 particle_offset = glm::vec2(20.0f, 0.0f);

		/** the name of the particle to render */
		std::string particle_name = "life";
		/** the sound to play */
		std::string heart_beat_sound = "heartbeat";
		/** the heart beat frequency */
		float heart_beat_frequency = 1.0f;

		/** the minimum alpha apply to particle when it is the last life */
		float fadeout_warning_base = 0.4f;

		/** the current heart warning timer value */
		float warning_value = 0.0f;
	};
	


















	// ====================================================================
	// GameHUDLevelTitleComponent
	// ====================================================================

	class GameHUDLevelTitleComponent : public GameHUDCacheValueComponent<Level const *>
	{
		friend class GameHUD;

	public:

		/** constructor */
		GameHUDLevelTitleComponent();
		/** constructor */
		GameHUDLevelTitleComponent(ParticleTextGenerator::GeneratorParams const & in_params) :
			GameHUDCacheValueComponent<Level const*>("%s", in_params) {}

	protected:

		/** override */
		virtual Level const * QueryValue() const override;
		/** override */
		virtual void UpdateTextMesh() override;
	};	

	// ====================================================================
	// GameHUDFreeCameraComponent
	// ====================================================================

	class GameHUDFreeCameraComponent : public GameHUDTextComponent
	{

	public:

		/** constructor */
		GameHUDFreeCameraComponent(char const* in_text = "Free Camera Mode");
		/** constructor */
		GameHUDFreeCameraComponent(char const* in_text, ParticleTextGenerator::GeneratorParams const & in_params);

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION