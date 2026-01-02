
#include "chaos/Chaos.h"


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

static chaos::EnumMetaData<Truc> const Truc_metadata =
{
	{ Truc::Truc1, "Truc1" },
	{ Truc::Truc2, "Truc2" },
	{ Truc::Truc3, "Truc3" },
	{ Truc::Truc4, "Truc4" }
};

//CHAOS_DECLARE_ENUM_METHOD(Truc);
CHAOS_IMPLEMENT_ENUM_METHOD(Truc, &Truc_metadata);

CHAOS_GLOBAL_VARIABLE(std::string, MyString);

CHAOS_GLOBAL_VARIABLE(Truc, mtruc);
CHAOS_GLOBAL_VARIABLE(bool, mybool);
CHAOS_GLOBAL_VARIABLE(int, myint, 666);
CHAOS_GLOBAL_VARIABLE(Truc, mytruc2, Truc::Truc3);
CHAOS_GLOBAL_VARIABLE(chaos::Hotpoint, hot);

CHAOS_GLOBAL_VARIABLE(AAA, aaa);


class MyApplicationTest : public chaos::WindowApplication
{
public:

	using chaos::WindowApplication::WindowApplication;

	virtual int MainBody() override
	{
		CreateNamedWindow("Global Variables", &chaos::ImGuiWindow::CreateImGuiWindow<chaos::ImGuiGlobalVariablesObject>);

		RunMessageLoop([this]()
		{
			return GetWindowCount() > 0;
		});
		return 0;
	}
};


int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<MyApplicationTest>(argc, argv, env);
}

