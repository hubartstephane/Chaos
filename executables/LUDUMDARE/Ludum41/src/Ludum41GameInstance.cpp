#include "Ludum41GameInstance.h"
#include "Ludum41Game.h"
#include "Ludum41Player.h"

LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
}

DEATH_GAMEFRAMEWORK_IMPLEMENT_GAMEINSTANCE(Ludum);

death::Player * LudumGameInstance::DoCreatePlayer()
{
	return new LudumPlayer(this);
}