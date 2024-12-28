
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

static chaos::EnumTools::EnumMetaData<Truc> const Truc_metadata =
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

#if 0

bool RuntimeCheck(chaos::LightweightFunction<bool()> func)
{
	bool result = func();
	assert(result);
	return result;
}

#define RUNTIME_CHECK(FUNC) static bool value = RuntimeCheck(FUNC);

static bool value = RuntimeCheck([]()
{
	std::vector<std::pair<char const*, char const*>> checks =
	{
		{CHAOS_PP_UNICODE_TO_UTF8(0, 0, 0, 2, 0), "\x20"},
		{CHAOS_PP_UNICODE_TO_UTF8(0, 0, 0, 4, 1), "\x41"},
		{CHAOS_PP_UNICODE_TO_UTF8(0, 0, 0, 7, F), "\x7F"},

		{CHAOS_PP_UNICODE_TO_UTF8(0, 0, 0, A, 9), "\xC2\xA9"},
		{CHAOS_PP_UNICODE_TO_UTF8(0, 0, 3, A, 9), "\xCE\xA9"},
		{CHAOS_PP_UNICODE_TO_UTF8(0, 0, 7, F, F), "\xDF\xBF"},

		{CHAOS_PP_UNICODE_TO_UTF8(0, 2, 0, A, C), "\xE2\x82\xAC"},
		{CHAOS_PP_UNICODE_TO_UTF8(0, 4, F, 6, 0), "\xE4\xBD\xA0"},
		{CHAOS_PP_UNICODE_TO_UTF8(0, F, F, F, F), "\xEF\xBF\xBF"},

		{CHAOS_PP_UNICODE_TO_UTF8(1, F, 6, 0, 0), "\xF0\x9F\x98\x80"},
		{CHAOS_PP_UNICODE_TO_UTF8(1, F, 6, 8, 0), "\xF0\x9F\x9A\x80"},
		{CHAOS_PP_UNICODE_TO_UTF8(1, 0, F, F, F, F), "\xF4\x8F\xBF\xBF"}
	};

	for (auto const& p : checks)
	{
		if (chaos::StringTools::Strcmp(p.first, p.second) != 0)
			return false;
	}
	return true;
});

#endif

class MyApplicationTest : public chaos::WindowApplication
{
public:

	using chaos::WindowApplication::WindowApplication;

	virtual int MainBody() override
	{
		CreateNamedWindow("Global Variables", &chaos::ImGuiWindow::CreateImGuiWindow<chaos::ImGuiGlobalVariablesObject>);

		return chaos::WindowApplication::MainBody();
	}
};


int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<MyApplicationTest>(argc, argv, env);
}

