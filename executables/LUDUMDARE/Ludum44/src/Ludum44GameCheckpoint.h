#pragma once

#include <death/GameCheckpoint.h>

#include <death/TiledMapLevel.h>



class LudumPlayerCheckpoint : public death::PlayerCheckpoint
{
public:

	float current_health = 1.0f;
	float current_max_health = 1.0f;

	size_t current_speed_index = 0;
	size_t current_damage_index = 0;
	size_t current_charged_damage_index = 0;
	size_t current_fire_rate_index = 0;
};

class LudumLevelCheckpoint : public death::TiledMap::TiledLevelCheckpoint
{
public:

	float scroll_factor = 1.0f;
};