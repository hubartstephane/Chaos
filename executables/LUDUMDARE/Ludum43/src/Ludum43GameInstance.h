#pragma once

#include <chaos/StandardHeaders.h> 
#include <death/Game.h>
#include <death/GameInstance.h>

// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public death::GameInstance
{

public:

	/** constructor */
	LudumGameInstance(death::Game * in_game);

	/** utility getters + cast */
	class LudumPlayer * GetLudumPlayer(int player_index);
	class LudumPlayer const * GetLudumPlayer(int player_index) const;

	class LudumGame * GetLudumGame();
	class LudumGame const * GetLudumGame() const;

protected:

	virtual death::Player * DoCreatePlayer() override;
};
