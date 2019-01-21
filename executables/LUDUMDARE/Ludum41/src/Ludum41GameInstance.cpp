#include "Ludum41GameInstance.h"
#include "Ludum41Game.h"
#include "Ludum41Player.h"

LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
}

death::Player * LudumGameInstance::DoCreatePlayer()
{
	return new LudumPlayer(this);
}