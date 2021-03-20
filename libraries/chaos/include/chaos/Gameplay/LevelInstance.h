#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class LevelInstance;

}; // namespace chaos

#else

namespace chaos
{

	// =====================================
	// LevelInstance : this is the runtime part of a level
	// =====================================

	class LevelInstance : public GPURenderable, public JSONSerializable
	{
		CHAOS_GAMEPLAY_ALLFRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS2(LevelInstance, GPURenderable);

	public:

		CHAOS_DECLARE_GAMEPLAY_GETTERS();

		/** destroy cameras when the level is finished */
		virtual void DestroyCameras();
		/** create cameras when the level is started */
		virtual void CreateCameras();

		/** get the transformation world to camera */
		obox2 GetCameraOBox(size_t index, bool apply_modifiers = true) const;
		/** the camera transform whenever no camera exists */
		obox2 GetDefaultCameraOBox() const;
		/** the initial camera box getter */		
		obox2 GetInitialCameraOBox(size_t index) const;

		/** returns level clock */
		Clock * GetLevelClock() { return level_clock.get();}
		/** returns level clock */
		Clock const * GetLevelClock() const { return level_clock.get(); }
		/** returns the level time */
		double GetLevelClockTime() const;

		/** get the world box */
		virtual box2 GetBoundingBox() const;

		/** some callbacks */
		virtual void OnLevelEnded();
		/** some callbacks */
		virtual void OnLevelStarted();

		/** returns whether the level is completed */
		virtual bool CheckLevelCompletion() const;
		/** returns whether we can go to next level */
		virtual bool CanCompleteLevel() const;

		/** restrict an object to the world */
		void RestrictObjectToWorld(ParticleAllocationBase * allocation, size_t index);
		/** restrict an pawn to the world */
		void RestrictPawnToWorld(PlayerPawn * player_pawn);

		/** get the current time out */
		float GetLevelTimeout() const { return level_timeout; }

		/** flag the level as completed */
		void SetLevelCompletionFlag();
		/** get the completion flag */
		bool GetLevelCompletionFlag() const;

		/** returns the sound category */
		SoundCategory * GetSoundCategory();
		/** returns the sound category */
		SoundCategory const * GetSoundCategory() const;

		/** the processor may save its configuration into a JSON file */
		virtual bool SerializeIntoJSON(nlohmann::json& json_entry) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool SerializeFromJSON(nlohmann::json const& json_entry) override;

	protected:

		/** the camera instanciation method */
		virtual Camera* DoCreateCamera();
		/** create a camera and initialize it */
		virtual Camera* CreateCamera();

		/** gets a camera by its index */
		Camera * DoGetCamera(size_t index, bool accept_freecam);
		/** gets a camera by its index */
		Camera const * DoGetCamera(size_t index, bool accept_freecam) const;

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual bool CanTick() override;

		/** additionnal initialization */
		virtual bool Initialize(Game * in_game, Level * in_level);

		/** called for each player whenever a level is started */
		virtual void OnPlayerEntered(Player * player);
		/** called for each player whenever a level is ended */
		virtual void OnPlayerLeaved(Player * player);

		/** called whenever the level is about to be restarted */
		virtual void OnRestart();

		/** create a player displacement component for the player */
		virtual PlayerDisplacementComponent* CreatePlayerDisplacementComponent(Player* player);

		/** create an instance of player displacement component */
		virtual PlayerDisplacementComponent* DoCreatePlayerDisplacementComponent();

		/** give the player a pawn */
		virtual PlayerPawn * CreatePlayerPawn(Player* player);
		/** the allocation PlayerPawn creation method */
		virtual PlayerPawn* DoCreatePlayerPawn();

		/** delete the pawn for the player */
		virtual void DestroyPlayerPawn(Player* player);
		/** delete the displacement component for the player */
		virtual void DestroyPlayerDisplacementComponent(Player* player);

		/** fill the rendering params before rendering */
		virtual void FillUniformProvider(GPUProgramProvider & main_uniform_provider);

		/** state changes */
		virtual void OnEnterPause();
		/** state changes */
		virtual void OnLeavePause();
		/** state changes */
		virtual void OnGameOver();

		/** check whether a player is dead according to the level instance */
		virtual bool IsPlayerDead(Player * player);

		/** change the background */
		virtual void CreateBackgroundImage();
		/** change the music */
		virtual void SetInGameMusic();

	protected:

		/** a pointer on the level that generated this */
		Level * level = nullptr;
		/** pointer on the game */
		Game * game = nullptr;
		/** the time to run the level (no time out by default) */
		float level_timeout = -1.0f;
		/** the level completion flag */
		bool level_completion_flag = false;

		/** a category for all sound started during that level */
		shared_ptr<SoundCategory> sound_category;

		/** the level clock */
		shared_ptr<Clock> level_clock;
		/** the camera */
		std::vector<shared_ptr<Camera>> cameras;

		/** class for the player pawn */
		SubClassOf<PlayerPawn> player_pawn_class;

		/** class for the player displacement component */
		SubClassOf<PlayerDisplacementComponent> player_displacement_component_class;

		/** class for the camera */
		SubClassOf<Camera> camera_class;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION