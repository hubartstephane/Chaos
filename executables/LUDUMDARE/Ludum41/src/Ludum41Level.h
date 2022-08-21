#pragma once

#include "chaos/Chaos.h"

// =================================================
// Levels
// =================================================

class LudumLevel : public chaos::Level
{

	CHAOS_DECLARE_OBJECT_CLASS(LudumLevel, chaos::Level);

public:

	static constexpr int NONE = -1;
	static constexpr int INDESTRUCTIBLE = -2;

	LudumLevel();

	/** get the number of brick in the level */
	size_t GetBrickCount() const;

public:

	/** indestructible brick count */
	size_t indestructible_brick_count = 0;
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
