#include "Ludum42TwoGameInstance.h"
#include "Ludum42TwoGame.h"
#include "Ludum42TwoPlayer.h"

LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
}

death::Player * LudumGameInstance::DoCreatePlayer()
{
	return new LudumPlayer(this);
}