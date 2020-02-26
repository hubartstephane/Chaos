#pragma once

// internal macros (do not use outside this file)
#define DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(prefix)\
	friend class prefix##PlayerCheckpoint;\
	friend class prefix##PlayerPawnCheckpoint;\
	friend class prefix##LevelCheckpoint;\
	friend class prefix##GameCheckpoint;\
	friend class prefix##Player;\
	friend class prefix##PawnPlayer;\
	friend class prefix##Level;\
	friend class prefix##LevelInstance;\
	friend class prefix##Game;\
	friend class prefix##Camera;\
	friend class prefix##CameraComponent;\
	friend class prefix##GameInstanceEntity;\
	friend class prefix##GameInstance;\
	friend class prefix##GameHUD;

// declare all friendship for main game classes
#define DEATH_GAMEFRAMEWORK_ALLFRIENDS() DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(BOOST_PP_EMPTY())






