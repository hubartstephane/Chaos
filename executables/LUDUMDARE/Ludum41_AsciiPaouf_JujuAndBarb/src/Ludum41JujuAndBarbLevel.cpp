#include "Ludum41JujuAndBarbPCH.h"
#include "Ludum41JujuAndBarbLevel.h"
#include "Ludum41JujuAndBarbLevelInstance.h"
#include "Ludum41JujuAndBarbGame.h"
#include "Ludum41JujuAndBarbGameInstance.h"
#include "Ludum41JujuAndBarbPlayer.h"

LudumLevel::LudumLevel()
{
	level_instance_class = LudumLevelInstance::GetStaticClass();
}

size_t LudumLevel::GetBrickCount() const
{
	size_t result = 0;
	for (size_t i = 0; i < bricks.size(); ++i)
	{
		std::vector<int> const & line = bricks[i];
		for (int b : line)
			if (b != LudumLevel::NONE)
				++result;
	}
	return result;
}