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

	/** constructor */
	LudumGameInstance(death::Game * in_game);

	DEATH_GAMEFRAMEWORK_DECLAREGAMEINSTANCEGETTERS(Ludum);

protected:

	virtual death::Player * DoCreatePlayer() override;
};
