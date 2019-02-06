#pragma once

#include <chaos/StandardHeaders.h> 
#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/GameFramework.h>

// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public death::GameInstance
{

public:

	DEATH_GAMEFRAMEWORK_DECLARE_GAMEINSTANCE(Ludum);

	/** constructor */
	LudumGameInstance(death::Game * in_game);

protected:

	virtual death::Player * DoCreatePlayer() override;
};
