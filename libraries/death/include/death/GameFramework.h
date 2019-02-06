#pragma once

#define DEATH_GAMEFRAMEWORK_DECLAREFRIENDSHIPS(prefix)\
	friend class prefix##Player;\
	friend class prefix##Level;\
	friend class prefix##LevelInstance;\
	friend class prefix##Game;\
	friend class prefix##GameInstance;\

#define DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, type)\
	prefix##type * Get##prefix##type();\
	prefix##type const * Get##prefix##type() const;

#define DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, classname, type)\
	prefix##type * prefix##classname::Get##prefix##type(){ return dynamic_cast<prefix##type *>(Get##type());}\
	prefix##type const * prefix##classname::Get##prefix##type() const { return dynamic_cast<prefix##type const *>(Get##type());}

#define DEATH_GAMEFRAMEWORK_DECLAREPLAYERGETTER(prefix)\
	prefix##Player * Get##prefix##Player(int player_index);\
	prefix##Player const * Get##prefix##Player(int player_index) const;

#define DEATH_GAMEFRAMEWORK_IMPLEMENTPLAYERGETTER(prefix, classname)\
	prefix##Player * prefix##classname::Get##prefix##Player(int player_index){ return dynamic_cast<prefix##Player *>(GetPlayer(player_index));}\
	prefix##Player const * prefix##classname::Get##prefix##Player(int player_index) const { return dynamic_cast<prefix##Player const*>(GetPlayer(player_index));}

// macro to be inserted in Game subclasses declaration
#define DEATH_GAMEFRAMEWORK_DECLAREGAMEGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLAREFRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, LevelInstance)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, GameInstance)\
	DEATH_GAMEFRAMEWORK_DECLAREPLAYERGETTER(prefix)

#define DEATH_GAMEFRAMEWORK_IMPLEMENTGAMEGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, Game, Level)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, Game, LevelInstance)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, Game, GameInstance)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTPLAYERGETTER(prefix, Game)

// macro to be inserted in Game Instance subclasses declaration
#define DEATH_GAMEFRAMEWORK_DECLAREGAMEINSTANCEGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLAREFRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, LevelInstance)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, Game)\
	DEATH_GAMEFRAMEWORK_DECLAREPLAYERGETTER(prefix)

#define DEATH_GAMEFRAMEWORK_IMPLEMENTGAMEINSTANCEGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, GameInstance, Level)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, GameInstance, LevelInstance)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, GameInstance, Game)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTPLAYERGETTER(prefix, GameInstance)

// macro to be inserted in Level Instance subclasses declaration
#define DEATH_GAMEFRAMEWORK_DECLARELEVELINSTANCEGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLAREFRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, Game)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, GameInstance)\
	DEATH_GAMEFRAMEWORK_DECLAREPLAYERGETTER(prefix)

#define DEATH_GAMEFRAMEWORK_IMPLEMENTLEVELINSTANCEGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, LevelInstance, Level)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, LevelInstance, Game)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, LevelInstance, GameInstance)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTPLAYERGETTER(prefix, LevelInstance)

// macro to be inserted in Player subclasses declaration
#define DEATH_GAMEFRAMEWORK_DECLAREPLAYERGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLAREFRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, LevelInstance)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, Game)\
	DEATH_GAMEFRAMEWORK_DECLAREGETTER(prefix, GameInstance)

#define DEATH_GAMEFRAMEWORK_IMPLEMENTPLAYERGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, Player, Level)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, Player, LevelInstance)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, Player, Game)\
	DEATH_GAMEFRAMEWORK_IMPLEMENTGETTER(prefix, Player, GameInstance)
