#include "Ludum45PCH.h"
#include "Ludum45Game.h"

class Machin
{

};

enum class Truc : int
{
	Truc1 = 6,
	Truc2 = 9,
	Truc3 = -1,
	Truc4 = 100
};


static chaos::EnumTools::EnumMetaData<Truc> const Truc_metadata =
{
	{ Truc::Truc1, "Truc1" },
	{ Truc::Truc2, "Truc2" },
	{ Truc::Truc3, "Truc3" },
	{ Truc::Truc4, "Truc4" }
};

CHAOS_IMPLEMENT_ENUM_METHOD(Truc, Truc_metadata);






CHAOS_GLOBAL_VARIABLE(Truc, mtruc);
CHAOS_GLOBAL_VARIABLE(bool, mybool);
CHAOS_GLOBAL_VARIABLE(int, myint, 666);
CHAOS_GLOBAL_VARIABLE(Truc, mytruc2, Truc::Truc3);
CHAOS_GLOBAL_VARIABLE(chaos::Hotpoint, hot);


#define TOTO(a, ...) 6 * a __VA_ARGS__

int main(int argc, char ** argv, char ** env)
{
	bool b1 = chaos::IsEnumBitmask<Truc>;
	bool b2 = chaos::IsEnumBitmask<chaos::Hotpoint>;

	std::ostringstream sss;
	sss << Truc::Truc4;

	auto s = sss.str();


	return chaos::RunGame<LudumGame>(argc, argv, env);
}
