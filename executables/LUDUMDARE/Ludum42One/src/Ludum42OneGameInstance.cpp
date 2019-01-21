#include "Ludum42OneGameInstance.h"
#include "Ludum42OneGame.h"
#include "Ludum42OnePlayer.h"

LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
}

death::Player * LudumGameInstance::DoCreatePlayer()
{
	return new LudumPlayer(this);
}