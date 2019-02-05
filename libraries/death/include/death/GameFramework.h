#pragma once

#define DEATH_GAMEFRAMEWORK_FRIENDSHIPS(prefix)\
	friend class prefix##Player;\
	friend class prefix##Level;\
	friend class prefix##LevelInstance;\
	friend class prefix##Game;\
	friend class prefix##GameInstance;\

#define DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, type)\
	prefix##type * Get##prefix##type(){ return dynamic_cast<prefix##type *>(Get##type());}\
	prefix##type const * Get##prefix##type() const { return dynamic_cast<prefix##type const *>(Get##type());}

#define DEATH_GAMEFRAMEWORK_TYPEDPLAYERGETTER(prefix)\
	prefix##Player * Get##prefix##Player(int player_index){ return dynamic_cast<prefix##Player *>(GetPlayer(player_index));}\
	prefix##Player const * Get##prefix##Player(int player_index) const { return dynamic_cast<prefix##Player const*>(GetPlayer(player_index));}\

// macro to be inserted in Game subclasses declaration
#define DEATH_GAMEFRAMEWORK_GAMEGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_FRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, LevelInstance)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, GameInstance)\
	DEATH_GAMEFRAMEWORK_TYPEDPLAYERGETTER(prefix)

// macro to be inserted in Game Instance subclasses declaration
#define DEATH_GAMEFRAMEWORK_GAMEINSTANCEGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_FRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, LevelInstance)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, Game)\
	DEATH_GAMEFRAMEWORK_TYPEDPLAYERGETTER(prefix)

// macro to be inserted in Level subclasses declaration
#define DEATH_GAMEFRAMEWORK_LEVELGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_FRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, LevelInstance)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, Game)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, GameInstance)\
	DEATH_GAMEFRAMEWORK_TYPEDPLAYERGETTER(prefix)

// macro to be inserted in Level Instance subclasses declaration
#define DEATH_GAMEFRAMEWORK_LEVELINSTANCEGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_FRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, Game)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, GameInstance)\
	DEATH_GAMEFRAMEWORK_TYPEDPLAYERGETTER(prefix)

// macro to be inserted in Player subclasses declaration
#define DEATH_GAMEFRAMEWORK_PLAYERGETTERS(prefix)\
	DEATH_GAMEFRAMEWORK_FRIENDSHIPS(prefix)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, Level)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, LevelInstance)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, Game)\
	DEATH_GAMEFRAMEWORK_TYPEDGETTER(prefix, GameInstance)
