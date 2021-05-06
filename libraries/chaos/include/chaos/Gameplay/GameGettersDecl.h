#ifdef CHAOS_FORWARD_DECLARATION

#define CHAOS_DECLARE_GAMEPLAY_GETTERS()\
	size_t GetPlayerCount() const;\
	AutoCastable<Player> GetPlayer(size_t player_index);\
	AutoConstCastable<Player> GetPlayer(size_t player_index) const;\
	AutoCastable<PlayerPawn> GetPlayerPawn(size_t player_index);\
	AutoConstCastable<PlayerPawn> GetPlayerPawn(size_t player_index) const;\
	AutoCastable<PlayerDisplacementComponent> GetPlayerDisplacementComponent(size_t player_index);\
	AutoConstCastable<PlayerDisplacementComponent> GetPlayerDisplacementComponent(size_t player_index) const;\
	AutoCastable<Game> GetGame();\
	AutoConstCastable<Game> GetGame() const;\
	AutoCastable<GameInstance> GetGameInstance();\
	AutoConstCastable<GameInstance> GetGameInstance() const;\
	AutoCastable<LevelInstance> GetLevelInstance();\
	AutoConstCastable<LevelInstance> GetLevelInstance() const;\
	AutoCastable<Level> GetLevel();\
	AutoConstCastable<Level> GetLevel() const;\
	size_t GetCameraCount() const;\
	AutoCastable<Camera> GetCamera(size_t camera_index);\
	AutoConstCastable<Camera> GetCamera(size_t camera_index) const;

#define CHAOS_IMPLEMENT_GAMEPLAY_GETTERS(CLASSNAME)\
	size_t CLASSNAME::GetPlayerCount() const { return GameGettersTools::GetPlayerCount(this);}\
	AutoCastable<Player> CLASSNAME::GetPlayer(size_t player_index) { return GameGettersTools::GetPlayer(this, player_index);}\
	AutoConstCastable<Player> CLASSNAME::GetPlayer(size_t player_index) const { return GameGettersTools::GetPlayer(this, player_index); }\
	AutoCastable<PlayerPawn> CLASSNAME::GetPlayerPawn(size_t player_index) { return GameGettersTools::GetPlayerPawn(this, player_index);}\
	AutoConstCastable<PlayerPawn> CLASSNAME::GetPlayerPawn(size_t player_index) const { return GameGettersTools::GetPlayerPawn(this, player_index); }\
	AutoCastable<PlayerDisplacementComponent> CLASSNAME::GetPlayerDisplacementComponent(size_t player_index) { return GameGettersTools::GetPlayerDisplacementComponent(this, player_index);}\
	AutoConstCastable<PlayerDisplacementComponent> CLASSNAME::GetPlayerDisplacementComponent(size_t player_index) const { return GameGettersTools::GetPlayerDisplacementComponent(this, player_index); }\
	AutoCastable<Game> CLASSNAME::GetGame() { return GameGettersTools::GetGame(this);}\
	AutoConstCastable<Game> CLASSNAME::GetGame() const { return GameGettersTools::GetGame(this); }\
	AutoCastable<GameInstance> CLASSNAME::GetGameInstance() { return GameGettersTools::GetGameInstance(this); }\
	AutoConstCastable<GameInstance> CLASSNAME::GetGameInstance() const { return GameGettersTools::GetGameInstance(this); }\
	AutoCastable<LevelInstance> CLASSNAME::GetLevelInstance() { return GameGettersTools::GetLevelInstance(this); }\
	AutoConstCastable<LevelInstance> CLASSNAME::GetLevelInstance() const { return GameGettersTools::GetLevelInstance(this); }\
	AutoCastable<Level> CLASSNAME::GetLevel() { return GameGettersTools::GetLevel(this); }\
	AutoConstCastable<Level> CLASSNAME::GetLevel() const { return GameGettersTools::GetLevel(this); }\
	size_t CLASSNAME::GetCameraCount() const { return GameGettersTools::GetCameraCount(this);}\
	AutoCastable<Camera> CLASSNAME::GetCamera(size_t camera_index) { return GameGettersTools::GetCamera(this, camera_index); }\
	AutoConstCastable<Camera> CLASSNAME::GetCamera(size_t camera_index) const { return GameGettersTools::GetCamera(this, camera_index); }

namespace chaos
{

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else

namespace chaos
{



}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION