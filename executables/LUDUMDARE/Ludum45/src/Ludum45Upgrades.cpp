#include "Ludum45Upgrades.h"

#include <death/game.h>
#include <death/soundcontext.h>

void PlayerUpgrade::Upgrade(class death::Game * game)
{
	if (level < max_level && game != nullptr)
	{
		if (!chaos::StringTools::IsEmpty(sound_name))
			game->Play(sound_name.c_str(), false, false, 0.0f, death::SoundContext::LEVEL);
		++level;
	}
}


