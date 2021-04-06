#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GameHUDComponent;
	class GameHUDMeshComponent;
	class GameHUDTextComponent;

	template<typename T, typename BASE>
	class GameHUDCacheValueComponent;
	template<typename T>
	class GameHUDCacheValueTextComponent;

	template<typename T>
	using GameHUDCacheValueMeshComponent = GameHUDCacheValueComponent<T, GameHUDMeshComponent>;

	class GameHUDNotificationComponent;
	class GameHUDScoreComponent;
	class GameHUDFramerateComponent;
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
		/** update the mesh according to internal data */
		virtual void UpdateMesh() {}
		/** invalidate the mesh. May force its reconstruction */
		void InvalidateMesh() { mesh = nullptr; }

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
		/** override */
		virtual void UpdateMesh() override;
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

	template<typename T, typename BASE>
	class GameHUDCacheValueComponent : public BASE
	{
		using type = T;

	protected:

		/** constructor */
		using BASE::BASE;

		/** gets the value (returns true whether the object is valid) */
		virtual bool QueryValue(type& result) const 
		{ 
			result = type(); 
			return true; 
		}
		/** override */
		virtual void OnInsertedInHUD()
		{
			if (!QueryValue(cached_value))
				InvalidateMesh();
			else
				UpdateMesh();
		}
		/** override */
		virtual bool DoUpdateGPUResources(GPURenderer* renderer) override
		{
			type new_value;
			if (!QueryValue(new_value))
				InvalidateMesh();
			else if (mesh == nullptr || cached_value != new_value) // maybe the Query may returns from false to true, but with a same cache value
			{                                                      // check if mesh was nullptr to avoid this case
				cached_value = new_value;
				UpdateMesh();
			}
			return true;
		}

	protected:

		/** the cached value */
		type cached_value;
	};

	// ====================================================================
	// GameHUDCacheValueTextComponent
	// ====================================================================

	template<typename T>
	class GameHUDCacheValueTextComponent : public GameHUDCacheValueComponent<T, GameHUDTextComponent>
	{
	protected:

		/** constructor */
		using GameHUDCacheValueComponent::GameHUDCacheValueComponent;

		/** override */
		virtual void UpdateMesh() override
		{
			SetText(StringTools::Printf(text.c_str(), cached_value).c_str());
		}
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

	class GameHUDScoreComponent : public GameHUDCacheValueTextComponent<int>
	{
	public:

		/** constructor */
		GameHUDScoreComponent(char const * in_text = "Score: %d");
		/** constructor */
		GameHUDScoreComponent(char const* in_text, ParticleTextGenerator::GeneratorParams const & in_params):
			GameHUDCacheValueTextComponent<int>(in_text, in_params) {}

	protected:

		/** override */
		virtual bool QueryValue(int & result) const override;
	};

	// ====================================================================
	// GameHUDFramerateComponent
	// ====================================================================

	class GameHUDFramerateComponent : public GameHUDCacheValueTextComponent<float>
	{
	public:

		/** constructor */
		GameHUDFramerateComponent(char const* in_text = "%02.01f FPS");
		/** constructor */
		GameHUDFramerateComponent(char const* in_text, ParticleTextGenerator::GeneratorParams const & in_params):
			GameHUDCacheValueTextComponent<float>(in_text, in_params) {}

	protected:

		/** override */
		virtual bool QueryValue(float& result) const override;
		/** override */
		virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params) override;

	protected:

		/** the framerate */
		float average_framerate = 0.0f;
	};

	// ====================================================================
	// GameHUDPerfsComponent
	// ====================================================================

	class GameHUDPerfsComponent : public GameHUDCacheValueTextComponent<std::pair<int, int>>
	{
	public:

		/** constructor */
		GameHUDPerfsComponent(char const* in_text = "DrawCalls(%03d) Vertices(%03d)");
		/** constructor */
		GameHUDPerfsComponent(char const* in_text, ParticleTextGenerator::GeneratorParams const& in_params) :
			GameHUDCacheValueTextComponent<std::pair<int, int>>(in_text, in_params) {}

	protected:

		/** override */
		virtual bool QueryValue(std::pair<int, int>& result) const override;
		/** override */
		virtual void UpdateMesh() override;
		/** override */
		virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params) override;

	protected:

		/** the drawcalls */
		int average_drawcall = 0;
		/** the vertices */
		int average_vertices = 0;
	};

	// ====================================================================
	// GameHUDTimeoutComponent
	// ====================================================================

	class GameHUDTimeoutComponent : public GameHUDCacheValueTextComponent<float>
	{
	public:

		/** constructor */
		GameHUDTimeoutComponent(char const * in_text = "%02.01f");
		/** constructor */
		GameHUDTimeoutComponent(char const* in_text, ParticleTextGenerator::GeneratorParams const & in_params) :
			GameHUDCacheValueTextComponent<float>(in_text, in_params) {}

	protected:

		/** override */
		virtual bool QueryValue(float & result) const override;
		/** override */
		virtual void TweakTextGeneratorParams(ParticleTextGenerator::GeneratorParams & final_params) const override;
	};


	// ====================================================================
	// GameHUDLifeComponent
	// ====================================================================

	class GameHUDLifeComponent : public GameHUDCacheValueMeshComponent<int>
	{
	public:

		/** constructor */
		GameHUDLifeComponent() = default;

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** tick heart */
		void TickHeartBeat(float delta_time);

		/** override */
		virtual bool QueryValue(int& result) const override;
		/** override */
		virtual void UpdateMesh() override;
		/** override */
		virtual bool InitializeFromConfiguration(nlohmann::json const& json, boost::filesystem::path const& config_path) override;

	protected:

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
		float warning_value = 1.0f;
	};
	
	// ====================================================================
	// GameHUDLevelTitleComponent
	// ====================================================================

	class GameHUDLevelTitleComponent : public GameHUDCacheValueTextComponent<Level const *>
	{
		friend class GameHUD;

	public:

		/** constructor */
		GameHUDLevelTitleComponent();
		/** constructor */
		GameHUDLevelTitleComponent(ParticleTextGenerator::GeneratorParams const & in_params) :
			GameHUDCacheValueTextComponent<Level const*>("%s", in_params) {}

	protected:

		/** override */
		virtual bool QueryValue(Level const * & result) const override;
		/** override */
		virtual void UpdateMesh() override;
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