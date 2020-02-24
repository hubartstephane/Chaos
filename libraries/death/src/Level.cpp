#include <death/Level.h>
#include <death/Game.h>
#include <death/LevelInstance.h>

namespace death
{
	// =====================================
	// LevelInstance implementation
	// =====================================

	LevelInstance * Level::CreateLevelInstance(Game * in_game)
	{
		// create the instance
		LevelInstance * result = DoCreateLevelInstance(in_game); 
		if (result == nullptr)
			return nullptr;
		// additional initialization
		if (!result->Initialize(in_game, this)) 
		{
			delete(result);
			return nullptr;
		}
		return result;
	}

	LevelInstance * Level::DoCreateLevelInstance(Game * in_game)
	{
		return new LevelInstance;
	}

}; // namespace death

