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
		LevelInstance * result = DoCreateLevelInstance(); 
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

	LevelInstance * Level::DoCreateLevelInstance()
	{
		return new LevelInstance;
	}

}; // namespace death

