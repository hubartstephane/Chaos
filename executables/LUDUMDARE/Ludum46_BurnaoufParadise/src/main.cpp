#include "Ludum46PCH.h"
#include "Ludum46Game.h"

int main(int argc, char ** argv, char ** env)
{
	chaos::GameApplicationData game_application_data;
	game_application_data.game_class = LudumGame::GetStaticClass();
	return chaos::RunApplication<chaos::GameApplication>(argc, argv, env, &game_application_data);
}
