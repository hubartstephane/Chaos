#include "Ludum45PCH.h"
#include "Ludum45Game.h"

enum class Truc : int
{
	Truc1,
	Truc2,
	Truc3,
	Truc4
};

CHAOS_GLOBAL_VARIABLE(chaos::ShaderType, mtruc);
CHAOS_GLOBAL_VARIABLE(bool, mybool);
CHAOS_GLOBAL_VARIABLE(int, myint, 666);


int main(int argc, char ** argv, char ** env)
{
	return chaos::RunGame<LudumGame>(argc, argv, env);
}
