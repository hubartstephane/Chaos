#include <chaos/Chaos.h>



int CHAOS_MAIN(int argc, char** argv, char** env)
{
	auto f = [argc](auto i)
	{
		return i * i + argc;
	};

	auto s = sizeof(f);


	return 0;
}
