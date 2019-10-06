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



	CHAOS_DECLARE_TAG(VIEW);
	CHAOS_DECLARE_TAG(POWER);
	CHAOS_DECLARE_TAG(DASH);
	CHAOS_DECLARE_TAG(SPECIALPOWER);
	CHAOS_DECLARE_TAG(GHOST);
};

class PlayerUpgrade : public chaos::ReferencedObject
{
public:

	PlayerUpgrade(chaos::TagType in_type, char const * in_bitmap_name):
		type(in_type),
		bitmap_name(in_bitmap_name)		
	{}

	void Upgrade();

public:

	chaos::TagType type = 0;

	std::string bitmap_name;

	int level = 0;
};