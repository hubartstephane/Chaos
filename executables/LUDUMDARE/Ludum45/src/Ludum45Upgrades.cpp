#include <chaos/Chaos.h>

#include "Ludum45Upgrades.h"


void PlayerUpgrade::Upgrade(class chaos::Game * game)
{
	if (level < max_level && game != nullptr)
	{
		if (!chaos::StringTools::IsEmpty(sound_name))
			game->PlaySound(sound_name.c_str(), false, false, 0.0f, chaos::SoundContext::LEVEL);
		++level;
	}
}


