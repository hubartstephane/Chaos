#include "Ludum41CustomPCH.h"
#include "Ludum41CustomGame.h"

int main(int argc, char** argv, char** env)
{
	return chaos::RunGame<LudumGame>(argc, argv, env);
}