#include "Ludum45PCH.h"
#include "Ludum45Game.h"

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunGame<LudumGame>(argc, argv, env);
}
