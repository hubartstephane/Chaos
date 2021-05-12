// XXX : Due to class declaration order and dependencies, this must only be included from .cpp !!
//       usefull for classes using CHAOS_IMPLEMENT_GAMEPLAY_GETTERS(...)

//  PlayerDisplacementComponent
//      ^
//      |
//      v
//    Player <----> GameInstance <----> Game <----> LevelInstance <----> Camera 
//      ^                                                |                 ^
//      |                                                |                 |
//      v                                                v                 v
//   PlayerPawn                                        Level         CameraComponent

namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GameGettersTools;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class GameGettersTools
	{
	public:


		// =======================================================================
		// Player Getters
		// =======================================================================

		template<typename T>
		static inline size_t GetPlayerCount(T const* src)
		{
			GameInstance const* gi = GetGameInstance(src);
			if (gi != nullptr)
				return gi->players.size();
			return 0;
		}

		template<typename T>
		static inline AutoCastable<Player> GetPlayer(T* src, size_t player_index)
		{
			assert(src != nullptr);
			GameInstance* gi = GetGameInstance(src);
			if (gi != nullptr)
			{
				if (player_index >= gi->players.size())
					return nullptr;
				return meta::get_raw_pointer(gi->players[player_index]);
			}
			return nullptr;
		}

		template<typename T>
		static inline AutoConstCastable<Player> GetPlayer(T const* src, size_t player_index)
		{
			assert(src != nullptr);
			GameInstance const* gi = GetGameInstance(src);
			if (gi != nullptr)
			{
				if (player_index >= gi->players.size())
					return nullptr;
				return meta::get_raw_pointer(gi->players[player_index]);
			}
			return nullptr;
		}

		// =======================================================================
		// PlayerPawn Getters
		// =======================================================================

		template<typename T>
		static inline AutoCastable<PlayerPawn> GetPlayerPawn(T* src, size_t player_index)
		{
			assert(src != nullptr);
			Player* pl = GetPlayer(src, player_index);
			if (pl != nullptr)
				return meta::get_raw_pointer(pl->pawn);
			return nullptr;
		}

		template<typename T>
		static inline AutoConstCastable<PlayerPawn> GetPlayerPawn(T const* src, size_t player_index)
		{
			assert(src != nullptr);
			Player const* pl = GetPlayer(src, player_index);
			if (pl != nullptr)
				return meta::get_raw_pointer(pl->pawn);
			return nullptr;
		}

		// =======================================================================
		// PlayerDisplacementComponent Getters
		// =======================================================================

		template<typename T>
		static inline AutoCastable<PlayerDisplacementComponent> GetPlayerDisplacementComponent(T* src, size_t player_index)
		{
			assert(src != nullptr);
			Player* pl = GetPlayer(src, player_index);
			if (pl != nullptr)
				return meta::get_raw_pointer(pl->displacement_component);
			return nullptr;
		}

		template<typename T>
		static inline AutoConstCastable<PlayerDisplacementComponent> GetPlayerDisplacementComponent(T const* src, size_t player_index)
		{
			assert(src != nullptr);
			Player const* pl = GetPlayer(src, player_index);
			if (pl != nullptr)
				return meta::get_raw_pointer(pl->displacement_component);
			return nullptr;
		}

		// =======================================================================
		// Camera Getters
		// =======================================================================

		template<typename T>
		static inline size_t GetCameraCount(T const* src)
		{
			LevelInstance const* li = GetLevelInstance(src);
			if (li != nullptr)
				return li->cameras.size();
			return 0;
		}

		template<typename T>
		static inline AutoCastable<Camera> GetCamera(T* src, size_t camera_index)
		{
			assert(src != nullptr);
			LevelInstance* li = GetLevelInstance(src);
			if (li != nullptr)
				return li->DoGetCamera(camera_index, true);  // accept free camera
			return nullptr;
		}

		template<typename T>
		static inline AutoConstCastable<Camera> GetCamera(T const* src, size_t camera_index)
		{
			assert(src != nullptr);
			LevelInstance const* li = GetLevelInstance(src);
			if (li != nullptr)
				return li->DoGetCamera(camera_index, true); // accept free camera
			return nullptr;
		}


		// =======================================================================
		// Game Getters
		// =======================================================================

		static inline AutoCastable<Game> GetGame(Player* src)
		{
			assert(src != nullptr);
			if (src->game_instance != nullptr)
				return GetGame(src->game_instance);
			return nullptr;
		}

		static inline AutoCastable<Game> GetGame(PlayerPawn* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetGame(src->player);
			return nullptr;
		}

		static inline AutoCastable<Game> GetGame(PlayerDisplacementComponent* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetGame(src->player);
			return nullptr;
		}

		static inline AutoCastable<Game> GetGame(GameInstance* src)
		{
			assert(src != nullptr);
			return meta::get_raw_pointer(src->game);
		}

		static inline AutoCastable<Game> GetGame(Game* src)
		{
			assert(src != nullptr);
			return src;
		}

		static inline AutoCastable<Game> GetGame(LevelInstance* src)
		{
			assert(src != nullptr);
			return meta::get_raw_pointer(src->game);
		}

		static inline AutoCastable<Game> GetGame(Camera* src)
		{
			assert(src != nullptr);
			if (src->level_instance != nullptr)
				return GetGame(src->level_instance);
			return nullptr;
		}

		static inline AutoCastable<Game> GetGame(CameraComponent* src)
		{
			assert(src != nullptr);
			if (src->camera != nullptr)
				return GetGame(src->camera);
			return nullptr;
		}

		static inline AutoConstCastable<Game> GetGame(Player const* src)
		{
			assert(src != nullptr);
			if (src->game_instance != nullptr)
				return GetGame(src->game_instance);
			return nullptr;
		}

		static inline AutoCastable<Game> GetGame(PlayerPawn const* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetGame(src->player);
			return nullptr;
		}

		static inline AutoCastable<Game> GetGame(PlayerDisplacementComponent const* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetGame(src->player);
			return nullptr;
		}

		static inline AutoConstCastable<Game> GetGame(GameInstance const* src)
		{
			assert(src != nullptr);
			return meta::get_raw_pointer(src->game);
		}

		static inline AutoConstCastable<Game> GetGame(Game const* src)
		{
			assert(src != nullptr);
			return src;
		}

		static inline AutoConstCastable<Game> GetGame(LevelInstance const* src)
		{
			assert(src != nullptr);
			return meta::get_raw_pointer(src->game);
		}

		static inline AutoConstCastable<Game> GetGame(Camera const* src)
		{
			assert(src != nullptr);
			if (src->level_instance != nullptr)
				return GetGame(src->level_instance);
			return nullptr;
		}

		static inline AutoConstCastable<Game> GetGame(CameraComponent const* src)
		{
			assert(src != nullptr);
			if (src->camera != nullptr)
				return GetGame(src->camera);
			return nullptr;
		}

		// =======================================================================
		// GameInstance Getters
		// =======================================================================

		static inline AutoCastable<GameInstance> GetGameInstance(Player* src)
		{
			assert(src != nullptr);
			return meta::get_raw_pointer(src->game_instance);
		}

		static inline AutoCastable<GameInstance> GetGameInstance(PlayerPawn* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetGameInstance(src->player);
			return nullptr;
		}

		static inline AutoCastable<GameInstance> GetGameInstance(PlayerDisplacementComponent* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetGameInstance(src->player);
			return nullptr;
		}

		static inline AutoCastable<GameInstance> GetGameInstance(GameInstance* src)
		{
			assert(src != nullptr);
			return src;
		}

		static inline AutoCastable<GameInstance> GetGameInstance(Game* src)
		{
			assert(src != nullptr);
			return meta::get_raw_pointer(src->game_instance);
		}

		static inline AutoCastable<GameInstance> GetGameInstance(LevelInstance* src)
		{
			assert(src != nullptr);
			if (src->game != nullptr)
				return GetGameInstance(src->game);
			return nullptr;
		}

		static inline AutoCastable<GameInstance> GetGameInstance(Camera* src)
		{
			assert(src != nullptr);
			if (src->level_instance != nullptr)
				return GetGameInstance(src->level_instance);
			return nullptr;
		}

		static inline AutoCastable<GameInstance> GetGameInstance(CameraComponent* src)
		{
			assert(src != nullptr);
			if (src->camera != nullptr)
				return GetGameInstance(src->camera);
			return nullptr;
		}

		static inline AutoConstCastable<GameInstance> GetGameInstance(Player const* src)
		{
			assert(src != nullptr);
			return meta::get_raw_pointer(src->game_instance);
		}

		static inline AutoCastable<GameInstance> GetGameInstance(PlayerPawn const* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetGameInstance(src->player);
			return nullptr;
		}

		static inline AutoCastable<GameInstance> GetGameInstance(PlayerDisplacementComponent const* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetGameInstance(src->player);
			return nullptr;
		}

		static inline AutoConstCastable<GameInstance> GetGameInstance(GameInstance const* src)
		{
			assert(src != nullptr);
			return src;
		}

		static inline AutoConstCastable<GameInstance> GetGameInstance(Game const* src)
		{
			assert(src != nullptr);
			return meta::get_raw_pointer(src->game_instance);
		}

		static inline AutoConstCastable<GameInstance> GetGameInstance(LevelInstance const* src)
		{
			assert(src != nullptr);
			if (src->game != nullptr)
				return GetGameInstance(src->game);
			return nullptr;
		}

		static inline AutoConstCastable<GameInstance> GetGameInstance(Camera const* src)
		{
			assert(src != nullptr);
			if (src->level_instance != nullptr)
				return GetGameInstance(src->level_instance);
			return nullptr;
		}

		static inline AutoConstCastable<GameInstance> GetGameInstance(CameraComponent const* src)
		{
			assert(src != nullptr);
			if (src->camera != nullptr)
				return GetGameInstance(src->camera);
			return nullptr;
		}

		// =======================================================================
		// LevelInstance Getters
		// =======================================================================

		static inline AutoCastable<LevelInstance> GetLevelInstance(Player* src)
		{
			assert(src != nullptr);
			if (src->game_instance != nullptr)
				return GetLevelInstance(src->game_instance);
			return nullptr;
		}

		static inline AutoCastable<LevelInstance> GetLevelInstance(PlayerPawn* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetLevelInstance(src->player);
			return nullptr;
		}

		static inline AutoCastable<LevelInstance> GetLevelInstance(PlayerDisplacementComponent* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetLevelInstance(src->player);
			return nullptr;
		}

		static inline AutoCastable<LevelInstance> GetLevelInstance(GameInstance* src)
		{
			assert(src != nullptr);
			if (src->game != nullptr)
				return GetLevelInstance(src->game);
			return nullptr;
		}

		static inline AutoCastable<LevelInstance> GetLevelInstance(Game* src)
		{
			assert(src != nullptr);
			return meta::get_raw_pointer(src->level_instance);
		}

		static inline AutoCastable<LevelInstance> GetLevelInstance(LevelInstance* src)
		{
			assert(src != nullptr);
			return src;
		}

		static inline AutoCastable<LevelInstance> GetLevelInstance(Camera* src)
		{
			assert(src != nullptr);
			return meta::get_raw_pointer(src->level_instance);
		}

		static inline AutoCastable<LevelInstance> GetLevelInstance(CameraComponent* src)
		{
			assert(src != nullptr);
			if (src->camera != nullptr)
				return GetLevelInstance(src->camera);
			return nullptr;
		}

		static inline AutoConstCastable<LevelInstance> GetLevelInstance(Player const* src)
		{
			assert(src != nullptr);
			if (src->game_instance != nullptr)
				return GetLevelInstance(src->game_instance);
			return nullptr;
		}

		static inline AutoCastable<LevelInstance> GetLevelInstance(PlayerPawn const* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetLevelInstance(src->player);
			return nullptr;
		}

		static inline AutoCastable<LevelInstance> GetLevelInstance(PlayerDisplacementComponent const* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetLevelInstance(src->player);
			return nullptr;
		}

		static inline AutoConstCastable<LevelInstance> GetLevelInstance(GameInstance const* src)
		{
			assert(src != nullptr);
			if (src->game != nullptr)
				return GetLevelInstance(src->game);
			return nullptr;
		}

		static inline AutoConstCastable<LevelInstance> GetLevelInstance(Game const* src)
		{
			assert(src != nullptr);
			return meta::get_raw_pointer(src->level_instance);
		}

		static inline AutoConstCastable<LevelInstance> GetLevelInstance(LevelInstance const* src)
		{
			assert(src != nullptr);
			return src;
		}

		static inline AutoConstCastable<LevelInstance> GetLevelInstance(Camera const* src)
		{
			assert(src != nullptr);
			return meta::get_raw_pointer(src->level_instance);
		}

		static inline AutoConstCastable<LevelInstance> GetLevelInstance(CameraComponent const* src)
		{
			assert(src != nullptr);
			if (src->camera != nullptr)
				return GetLevelInstance(src->camera);
			return nullptr;
		}

		// =======================================================================
		// Level Getters
		// =======================================================================

		template<typename T>
		static inline AutoCastable<Level> GetLevel(T* src)
		{
			assert(src != nullptr);
			LevelInstance* li = GetLevelInstance(src);
			if (li != nullptr)
				return meta::get_raw_pointer(li->level);
			return nullptr;
		}

		template<typename T>
		static inline AutoConstCastable<Level> GetLevel(T const* src)
		{
			assert(src != nullptr);
			LevelInstance const* li = GetLevelInstance(src);
			if (li != nullptr)
				return meta::get_raw_pointer(li->level);
			return nullptr;
		}

	};

#endif

}; // namespace chaos