#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/BitmapAtlas.h>
#include <chaos/ReferencedObject.h>
#include <chaos/NamedObject.h>

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>
#include <death/GameFramework.h>

namespace UpgradeKeys
{
	CHAOS_DECLARE_TAG(SPEED);
	CHAOS_DECLARE_TAG(DAMAGE);
	CHAOS_DECLARE_TAG(DASH);
	CHAOS_DECLARE_TAG(GHOST);
	CHAOS_DECLARE_TAG(POWERRATE);
	CHAOS_DECLARE_TAG(POWERSPREAD);
	CHAOS_DECLARE_TAG(SPECIALPOWER);
};

class PlayerUpgrade : public chaos::ReferencedObject
{
public:

	PlayerUpgrade(chaos::TagType in_type, int in_max_level, char const * in_bitmap_name, char const * in_sound_name):
		type(in_type),
		max_level(in_max_level),
		bitmap_name(in_bitmap_name),
		sound_name(in_sound_name)
	{}

	void Upgrade(class death::Game * game);

public:

	chaos::TagType type = 0;
	int level = 0;
	int max_level = 0;

	std::string bitmap_name;
	std::string sound_name;
};