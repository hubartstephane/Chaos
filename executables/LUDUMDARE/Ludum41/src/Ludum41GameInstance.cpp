#include "Ludum41GameInstance.h"
#include "Ludum41Game.h"
#include "Ludum41Level.h"
#include "Ludum41LevelInstance.h"
#include "Ludum41Player.h"

DEATH_GAMEFRAMEWORK_IMPLEMENT_GAMEINSTANCE(Ludum);

LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
}

death::Player * LudumGameInstance::DoCreatePlayer()
{
	return new LudumPlayer(this);
}