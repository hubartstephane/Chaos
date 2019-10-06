#include "Ludum45Upgrades.h"

void PlayerUpgrade::Upgrade()
{
	if (level < max_level)
		++level;
}


