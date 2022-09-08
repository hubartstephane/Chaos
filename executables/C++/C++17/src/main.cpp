#include "chaos/Chaos.h"

CHAOS_APPLICATION_ARG(bool, mybool);
CHAOS_APPLICATION_ARG(int, myint, 666);
CHAOS_APPLICATION_ARG(std::string, mystring);
CHAOS_APPLICATION_ARG(std::vector<int>, myvectorint);

int main(int argc, char** argv, char** env)
{
	chaos::ApplicationArgumentManager::GetInstance()->ParseArguments(argc, argv);

	auto  b = Arguments::mybool.Get();
	auto  i = Arguments::myint.Get();
	auto  s = Arguments::mystring.Get();
	auto vi = Arguments::myvectorint.Get();


	return 0;
}


