namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GameHUD;
	class MainMenuHUD;
	class PauseMenuHUD;
	class GameOverHUD;
	class PlayingHUD;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =============================================
	// GameHUD
	// =============================================

	class CHAOS_API GameHUD : public GPURenderable, public ConfigurationUserInterface
	{
		CHAOS_GAMEPLAY_ALLFRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(GameHUD, GPURenderable);

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
		T* FindComponentByClass()
		{
			for (auto it = components.begin(); it != components.end(); ++it)
			{
				T* result = auto_cast(it->second.get());
				if (result != nullptr)
					return result;
			}
			return nullptr;
		}
		/** find a component by its class */
		template<typename T>
		T const* FindComponentByClass() const
		{
			for (auto it = components.begin(); it != components.end(); ++it)
			{
				T* result = auto_cast(it->second.get());
				if (result != nullptr)
					return result;
			}
			return nullptr;
		}

		/** initialization of the HUD from the game values */
		virtual bool Initialize(Game* in_game);

		/** insert a component inside the HUD */
		void RegisterComponent(GameHUDComponent* in_component);
		/** remove a component from the HUD */
		void UnregisterComponent(TagType key);
		/** remove a component from the HUD */
		void UnregisterComponent(GameHUDComponent* in_component);

		/** get the component from its ID */
		GameHUDComponent* FindComponent(TagType key);
		/** get the component from its ID */
		GameHUDComponent const* FindComponent(TagType key) const;

		/** clear all components from the HUD */
		void Clear();

	protected:

		/** create the particles */
		virtual bool FillHUDContent();

		/** component creation */
		template<typename T, typename... PARAMS>
		T * CreateHUDComponent(TagType key, PARAMS && ...params)
		{
			if (T * result = new T(std::forward<PARAMS>(params)...))
			{
				GiveChildConfiguration(result, (char const*)key);
				result->SetTag(key);

				if (!result->Initialize())
				{
					delete(result);
					return nullptr;
				}
				return result;
			}
			return nullptr;
		}

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual int DoDisplay(GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params) override;

	protected:

		/** the allocations */
		std::map<TagType, shared_ptr<GameHUDComponent>> components;
		/** the game owning the HUD */
		class Game* game = nullptr;
	};

	// =============================================
	// MainMenuHUD
	// =============================================

	class CHAOS_API MainMenuHUD : public GameHUD
	{
		CHAOS_DECLARE_OBJECT_CLASS(MainMenuHUD, GameHUD);

	protected:

		/** override */
		virtual bool FillHUDContent() override;
	};

	// =============================================
	// PauseMenuHUD
	// =============================================

	class CHAOS_API PauseMenuHUD : public GameHUD
	{

		CHAOS_DECLARE_OBJECT_CLASS(PauseMenuHUD, GameHUD);

	protected:

		/** override */
		virtual bool FillHUDContent() override;
	};

	// =============================================
	// GameOverHUD
	// =============================================

	class CHAOS_API GameOverHUD : public GameHUD
	{

		CHAOS_DECLARE_OBJECT_CLASS(GameOverHUD, GameHUD);

	protected:

		/** override */
		virtual bool FillHUDContent() override;
	};

	// =============================================
	// PlayingHUD
	// =============================================

	class CHAOS_API PlayingHUD : public GameHUD
	{

		CHAOS_DECLARE_OBJECT_CLASS(PlayingHUD, GameHUD);

	protected:

		/** override */
		virtual bool FillHUDContent() override;
	};

#endif

}; // namespace chaos