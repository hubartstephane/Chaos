#include "chaos/Chaos.h"

CHAOS_APPLICATION_ARG(bool, mybool);
CHAOS_APPLICATION_ARG(int, myint, 666);
CHAOS_APPLICATION_ARG(std::string, mystring);
CHAOS_APPLICATION_ARG(std::vector<int>, myvectorint);

int main(int argc, char** argv, char** env)
{
	chaos::ApplicationArgumentManager::GetInstance()->ParseArguments(argc, argv);

	auto  b = mybool.Get();
	auto  i = myint.Get();
	auto  s = mystring.Get();
	auto vi = myvectorint.Get();


#if 0
	chaos::Object o1, o2;
	o1.AddReference();

	o1 = o2;
#endif


	return 0;
}


