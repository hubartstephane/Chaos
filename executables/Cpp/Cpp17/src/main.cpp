#include "chaos/Chaos.h"

CHAOS_GLOBAL_VARIABLE(bool, mybool);
CHAOS_GLOBAL_VARIABLE(int, myint, 666);
CHAOS_GLOBAL_VARIABLE(std::string, mystring);
CHAOS_GLOBAL_VARIABLE(std::vector<int>, myvectorint);

int main(int argc, char** argv, char** env)
{
	chaos::GlobalVariableManager::GetInstance()->ParseArguments(argc, argv);

	auto  b = mybool.Get();
	auto  i = myint.Get();
	auto  s = mystring.Get();
	auto vi = myvectorint.Get();

	return 0;
}


