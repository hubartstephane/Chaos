namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Player;
	class PlayerGamepadCallbacks;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =============================================
	// Player
	// =============================================

	class CHAOS_API Player : public Tickable, public InputReceiverInterface, public JSONSerializableInterface, public ConfigurationUserInterface
	{
		friend class PlayerGamepadCallbacks;

		CHAOS_GAMEPLAY_ALLFRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(Player, Tickable);

	public:

		CHAOS_DECLARE_GAMEPLAY_GETTERS();

		/** override */
		virtual bool TraverseInputReceiver(InputReceiverTraverser & in_traverser) override;
		/** override */
		virtual bool EnumerateInputActions(InputActionProcessor& in_action_processor, EnumerateInputActionContext in_context) override;

		/** get the index of the player */
		size_t GetPlayerIndex() const { return player_index; }

		/** get the current score */
		int GetScore() const { return score; }
		/** get the life count */
		int GetLifeCount() const { return life_count; }
		/** get the current health value */
		float GetHealth() const { return health; }
		/** get the maximum health value health value */
		float GetMaxHealth() const { return max_health; }

		/** update player score */
		void SetScore(int in_score, bool increment = false);
		/** update player life */
		void SetLifeCount(int in_life, bool increment = false);
		/** update player health */
		void SetHealth(float in_value, bool in_increment = false);

		/** check whether the player is dead */
		virtual bool IsDead() const;

		/** try to give physical device to the player */
		bool CapturePhysicalGamepad(PhysicalGamepad* in_physical_gamepad);

		/** gets the pawn */
		AutoCastable<PlayerPawn> GetPawn() { return pawn.get(); }
		/** gets the pawn */
		AutoConstCastable<PlayerPawn> GetPawn() const { return pawn.get(); }

		/** gets the displacement component */
		AutoCastable<PlayerDisplacementComponent> GetDisplacementComponent() { return displacement_component.get(); }
		/** gets the pawn */
		AutoConstCastable<PlayerDisplacementComponent> GetDisplacementComponent() const { return displacement_component.get(); }

		/** the processor may save its configuration into a JSON file */
		virtual bool SerializeIntoJSON(nlohmann::json * json) const override;
		/** the processor may save its configuration from a JSON file */
		virtual bool SerializeFromJSON(JSONReadConfiguration config) override;

	protected:

		/** initialize the game instance */
		virtual bool Initialize(GameInstance* in_game_instance);

		/** called whenever the level is being changed */
		virtual void OnLevelChanged(Level* new_level, Level* old_level, LevelInstance* new_level_instance);

		/** override */
		virtual bool DoTick(float delta_time) override;

		/** called whenever player gamepad is disconnected */
		virtual void OnGamepadDisconnected();

		/** tick player method when playing */
		virtual void TickInternal(float delta_time);
		/** tick the player displacement */
		virtual void TickPlayerDisplacement(float delta_time);
		/** tick the invulnerability */
		virtual void TickInvulnerability(float delta_time);

		/** called whenever a life is lost */
		virtual void OnLifeLost();

		/** called whenever health is changed */
		virtual void OnHealthChanged(float old_health, float new_health, bool invulnerable);

		/** the effects for life changes */
		virtual void PlayHealthChangedEffects(bool health_lost);

		/** override */
		virtual bool OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context) override;

		/** override */
		virtual bool InvokeWithUpgradedInputDevice(InputDeviceInterface const * in_input_device, InvokeWithUpgradedInputDeviceFunction in_func) override;

	protected:

		/** the game instance owning the player */
		GameInstance* game_instance = nullptr;

		/** a gamepad that can be given to the player */
		shared_ptr<Gamepad> gamepad;

		/** the player pawn */
		shared_ptr<PlayerPawn> pawn;
		/** the player displacement component */
		shared_ptr<PlayerDisplacementComponent> displacement_component;

		/** the index of the player */
		size_t player_index = 0;

		/** the settings for gamepad filtering */
		GamepadInputFilterSettings gamepad_filter_settings;

		/** the current life for the player */
		int life_count = 3;
		/** the current health of the player */
		float health = 1.0f;
		/** the maximum health of the player */
		float max_health = 1.0f;
		/** the current timer of invulnerability */
		float invulnerability_timer = 0.0f;
		/** the invulnerability after some health is lost */
		float invulnerability_duration = 1.0f;
		/** the score for the player */
		int score = 0;
	};

	// =============================================
	// A callback object for gamepad disconnection
	// =============================================

	class CHAOS_API PlayerGamepadCallbacks : public GamepadCallbacks
	{

	public:

		/** constructor */
		PlayerGamepadCallbacks(Player* in_player);
		/** override */
		virtual bool OnGamepadDisconnected(Gamepad* in_gamepad) override;

	protected:

		/** player of interrest */
		Player* player = nullptr;
	};

#endif

}; // namespace chaos