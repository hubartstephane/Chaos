#include "Ludum47PCH.h"
#include "Ludum47Game.h"

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunGame<LudumGame>(argc, argv, env);
}