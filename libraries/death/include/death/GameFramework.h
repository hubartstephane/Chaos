#pragma once

// declare all friendship for main game classes
#define DEATH_GAMEFRAMEWORK_ALLFRIENDS()\
	friend class Player;\
	friend class GameLevel;\
	friend class GameLevelInstance;\
	friend class Game;\
	friend class GameInstance;

// internal macros (do not use outside this file)
#define DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(prefix)\
	friend class prefix##Player;\
	friend class prefix##Level;\
	friend class prefix##LevelInstance;\
	friend class prefix##Game;\
	friend class prefix##GameInstance;

#define DEATH_GAMEFRAMEWORK_DECLARE_GETTER(prefix, type)\
	prefix##type * Get##prefix##type();\
	prefix##type const * Get##prefix##type() const;

#define DEATH_GAMEFRAMEWORK_IMPLEMENT_GETTER(prefix, classname, type)\
	prefix##type * prefix##classname::Get##prefix##type(){ return dynamic_cast<prefix##type *>(Get##type());}\
	prefix##type const * prefix##classname::Get##prefix##type() const { return dynamic_cast<prefix##type const *>(Get##type());}

#define DEATH_GAMEFRAMEWORK_DECLARE_PLAYERGETTER(prefix)\
	prefix##Player * Get##prefix##Player(int player_index);\
	prefix##Player const * Get##prefix##Player(int player_index) const;

#define DEATH_GAMEFRAMEWORK_IMPLEMENT_PLAYERGETTER(prefix, classname)\
	prefix##Player * prefix##classname::Get##prefix##Player(int player_index){ return dynamic_cast<prefix##Player *>(GetPlayer(player_index));}\
	prefix##Player const * prefix##classname::Get##prefix##Player(int player_index) const { return dynamic_cast<prefix##Player const*>(GetPlayer(player_index));}

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
