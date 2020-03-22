#pragma once

// internal macros (do not use outside this file)
#define DEATH_GAMEFRAMEWORK_DECLARE_UTILITY(prefix, friend_prefix)\
	friend_prefix class prefix##PlayerCheckpoint;\
	friend_prefix class prefix##PlayerPawnCheckpoint;\
	friend_prefix class prefix##LevelCheckpoint;\
	friend_prefix class prefix##GameCheckpoint;\
	friend_prefix class prefix##Player;\
	friend_prefix class prefix##PlayerPawn;\
	friend_prefix class prefix##Level;\
	friend_prefix class prefix##LevelInstance;\
	friend_prefix class prefix##Game;\
	friend_prefix class prefix##Camera;\
	friend_prefix class prefix##CameraComponent;\
	friend_prefix class prefix##GameInstance;\
	friend_prefix class prefix##GameHUD;\
	friend_prefix class GameEntityTools;

// internal macros (do not use outside this file)
#define DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(prefix) DEATH_GAMEFRAMEWORK_DECLARE_UTILITY(prefix, friend)

// declare all friendship for main game classes
#define DEATH_GAMEFRAMEWORK_ALLFRIENDS() DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(BOOST_PP_EMPTY())

// declare main classes for death
namespace death
{
	DEATH_GAMEFRAMEWORK_DECLARE_UTILITY(BOOST_PP_EMPTY(), BOOST_PP_EMPTY())

}; // namespace death




