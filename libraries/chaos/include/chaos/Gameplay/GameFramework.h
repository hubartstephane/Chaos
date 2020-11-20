#ifdef CHAOS_FORWARD_DECLARATION


// internal macros (do not use outside this file)
#define CHAOS_DECLARE_GAMEPLAY_UTILITY(prefix, friend_prefix)\
	friend_prefix class prefix##LevelCheckpoint;\
	friend_prefix class prefix##GameCheckpoint;\
	friend_prefix class prefix##Player;\
	friend_prefix class prefix##PlayerPawn;\
	friend_prefix class prefix##PlayerDisplacementComponent;\
	friend_prefix class prefix##Level;\
	friend_prefix class prefix##LevelInstance;\
	friend_prefix class prefix##Game;\
	friend_prefix class prefix##Camera;\
	friend_prefix class prefix##CameraComponent;\
	friend_prefix class prefix##GameInstance;\
	friend_prefix class prefix##GameApplication;\
	friend_prefix class prefix##GameHUD;\
	friend_prefix class GameGettersTools;

// internal macros (do not use outside this file)
#define CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(prefix) CHAOS_DECLARE_GAMEPLAY_UTILITY(prefix, friend)

// declare all friendship for main game classes
#define CHAOS_GAMEPLAY_ALLFRIENDS CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(BOOST_PP_EMPTY())

// declare main classes for death
namespace chaos
{
	CHAOS_DECLARE_GAMEPLAY_UTILITY(BOOST_PP_EMPTY(), BOOST_PP_EMPTY())

}; // namespace chaos


#else













#endif // CHAOS_FORWARD_DECLARATION