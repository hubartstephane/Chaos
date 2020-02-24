#pragma once

// internal macros (do not use outside this file)
#define DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(prefix)\
	friend class prefix##PlayerCheckpoint;\
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
#define DEATH_GAMEFRAMEWORK_ALLFRIENDS()\
	friend class PlayerCheckpoint;\
	friend class LevelCheckpoint;\
	friend class GameCheckpoint;\
	friend class Player;\
	friend class PlayerPawn;\
	friend class GameLevel;\
	friend class GameLevelInstance;\
	friend class Game;\
	friend class Camera;\
	friend class CameraComponent;\
	friend class GameInstanceEntity;\
	friend class GameInstance;\
	friend class GameHUD;


#define DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, type)\
	prefix##type * Get##prefix##type();\
	prefix##type const * Get##prefix##type() const;

#define DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, classname, type)\
	prefix##type * prefix##classname::Get##prefix##type(){ return auto_cast(Get##type());}\
	prefix##type const * prefix##classname::Get##prefix##type() const { return auto_cast(Get##type());}

#define DEATH_GAMEFRAMEWORK_DECLARE_PLAYERGETTER(prefix)\
	prefix##Player * Get##prefix##Player(size_t player_index);\
	prefix##Player const * Get##prefix##Player(size_t player_index) const;

#define DEATH_GAMEFRAMEWORK_IMPLEMENT_PLAYERGETTER(prefix, classname)\
	prefix##Player * prefix##classname::Get##prefix##Player(size_t player_index){ return auto_cast(GetPlayer(player_index));}\
	prefix##Player const * prefix##classname::Get##prefix##Player(size_t player_index) const { return auto_cast(GetPlayer(player_index));}

// macro to be inserted in Game subclasses declaration
#define DEATH_GAMEFRAMEWORK_DECLARE_GAME(prefix)\
	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, LevelInstance)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, GameInstance)\
	DEATH_GAMEFRAMEWORK_DECLARE_PLAYERGETTER(prefix)

#define DEATH_GAMEFRAMEWORK_IMPLEMENT_GAME(prefix)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, Game, Level)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, Game, LevelInstance)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, Game, GameInstance)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_PLAYERGETTER(prefix, Game)

// macro to be inserted in Game Instance subclasses declaration
#define DEATH_GAMEFRAMEWORK_DECLARE_GAMEINSTANCE(prefix)\
	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, LevelInstance)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, Game)\
	DEATH_GAMEFRAMEWORK_DECLARE_PLAYERGETTER(prefix)

#define DEATH_GAMEFRAMEWORK_IMPLEMENT_GAMEINSTANCE(prefix)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, GameInstance, Level)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, GameInstance, LevelInstance)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, GameInstance, Game)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_PLAYERGETTER(prefix, GameInstance)

// macro to be inserted in Level Instance subclasses declaration
#define DEATH_GAMEFRAMEWORK_DECLARE_LEVELINSTANCE(prefix)\
	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, Game)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, GameInstance)\
	DEATH_GAMEFRAMEWORK_DECLARE_PLAYERGETTER(prefix)

#define DEATH_GAMEFRAMEWORK_IMPLEMENT_LEVELINSTANCE(prefix)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, LevelInstance, Level)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, LevelInstance, Game)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, LevelInstance, GameInstance)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_PLAYERGETTER(prefix, LevelInstance)

// macro to be inserted in Player subclasses declaration
#define DEATH_GAMEFRAMEWORK_DECLARE_PLAYER(prefix)\
	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, LevelInstance)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, Game)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, GameInstance)

#define DEATH_GAMEFRAMEWORK_IMPLEMENT_PLAYER(prefix)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, Player, Level)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, Player, LevelInstance)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, Player, Game)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, Player, GameInstance)

// macro to be inserted in Player subclasses declaration
#define DEATH_GAMEFRAMEWORK_DECLARE_HUD(prefix)\
	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, LevelInstance)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, Game)\
	DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, GameInstance)\
	DEATH_GAMEFRAMEWORK_DECLARE_PLAYERGETTER(prefix)

#define DEATH_GAMEFRAMEWORK_IMPLEMENT_HUD(prefix)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, PlayingHUD, Level)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, PlayingHUD, LevelInstance)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, PlayingHUD, Game)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, PlayingHUD, GameInstance)\
	DEATH_GAMEFRAMEWORK_IMPLEMENT_PLAYERGETTER(prefix, PlayingHUD)
