#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>

#include <death/Game.h>
#include <death/GameLevel.h>
#include <death/GameFramework.h>

// =================================================
// Levels
// =================================================

class LudumLevel : public death::GameLevel
{

public:

	static int const NONE = -1;
	static int const INDESTRUCTIBLE = -2;

	/** get the number of brick in the level */
	size_t GetBrickCount() const;

protected:

	/** override */
	virtual death::GameLevelInstance * DoCreateLevelInstance(death::Game * in_game) override;

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
