#pragma once

#include <chaos/Chaos.h>

#include <death/Game.h>
#include <death/Level.h>
#include <death/GameFramework.h>

// =================================================
// Levels
// =================================================

class LudumLevel : public death::Level
{
	CHAOS_OBJECT_DECLARE_CLASS2(LudumLevel, death::Level);

public:

	
	static constexpr int INDESTRUCTIBLE = -4;
	static constexpr int FOUR_LIFE = -3;
	static constexpr int TWO_LIFE = -2;
	static constexpr int NONE = -1;

	LudumLevel();
	
	/** get the number of brick in the level */
	size_t GetBrickCount() const;

public:

	/** indestructible brick count */
	size_t indestructible_brick_count = 0;
	/** destructible brick count */
	size_t destructible_brick_count = 0;
	/** the brick types */
	std::vector<std::vector<int>> bricks;
	/** music */
	std::string music;
	/** background */
	std::string background_material;
	std::string background_texture;
	/** dictionnary */
	std::vector<std::string> dictionnary;
};
