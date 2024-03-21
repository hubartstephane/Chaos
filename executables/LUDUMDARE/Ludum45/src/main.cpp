#include "Ludum45PCH.h"
#include "Ludum45Game.h"

enum class Truc : int
{
	None = 0,
	Truc1 = 1,
	Truc2 = 2,
	Truc3 = 4,
	Truc4 = 8
};

class AAA
{
public:

	void DrawImGuiVariable(chaos::DrawImGuiVariableFlags flags)
	{
		if (ImGui::CollapsingHeader("Configuration"))
		{
			chaos::DrawImGuiVariable(i, flags);
			chaos::DrawImGuiVariable(j, flags);
			chaos::DrawImGuiVariable(name, flags);
		}
	}

	int i = 666;
	int j = 667;
	std::string name;

};




static chaos::EnumTools::EnumMetaData<Truc> const Truc_metadata =
{
	{ Truc::Truc1, "Truc1" },
	{ Truc::Truc2, "Truc2" },
	{ Truc::Truc3, "Truc3" },
	{ Truc::Truc4, "Truc4" }
};

//CHAOS_DECLARE_ENUM_METHOD(Truc);
CHAOS_IMPLEMENT_ENUM_METHOD(Truc, &Truc_metadata);

CHAOS_GLOBAL_VARIABLE(Truc, mtruc);
CHAOS_GLOBAL_VARIABLE(bool, mybool);
CHAOS_GLOBAL_VARIABLE(int, myint, 666);
CHAOS_GLOBAL_VARIABLE(Truc, mytruc2, Truc::Truc3);
CHAOS_GLOBAL_VARIABLE(chaos::Hotpoint, hot);

CHAOS_GLOBAL_VARIABLE(AAA, aaa);

// ===============================================================================


int main(int argc, char ** argv, char ** env)
{
	Matrix m;
	for (size_t i = 0; i < 4; ++i)
		m(i, i) = new MatrixComponent(float(i + 1));

	for (size_t y = 0; y < 4; ++y)
	{
		for (size_t x = 0; x < 4; ++x)
		{
			std::string c = ToString(m(x, y));
		}
	}

	return chaos::RunGame<LudumGame>(argc, argv, env);
}
