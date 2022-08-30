#pragma once

#include "Ludum45PCH.h"

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

class PlayerUpgrade : public chaos::Object
{
public:

	PlayerUpgrade(chaos::TagType in_type, size_t in_max_level, char const * in_bitmap_name, char const * in_sound_name):
		type(in_type),
		max_level(in_max_level),
		bitmap_name(in_bitmap_name),
		sound_name(in_sound_name)
	{}

	void Upgrade(class chaos::Game * game);

public:

	chaos::TagType type = 0;
	size_t level = 0;
	size_t max_level = 0;

	std::string bitmap_name;
	std::string sound_name;
};