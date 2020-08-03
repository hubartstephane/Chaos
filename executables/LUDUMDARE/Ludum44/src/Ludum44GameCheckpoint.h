#pragma once

#include <death/GameCheckpoint.h>

#include <death/TiledMapLevel.h>


class LudumLevelCheckpoint : public death::TiledMapLevelCheckpoint
{
public:

	float scroll_factor = 1.0f;
};