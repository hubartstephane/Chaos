#include "Ludum43GameInstance.h"
#include "Ludum43Game.h"
#include "Ludum43Player.h"

LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
}

death::Player * LudumGameInstance::DoCreatePlayer()
{
	return new LudumPlayer(this);
}