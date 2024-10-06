#include "chaos/Chaos.h"


int main(int argc, char** argv, char** env)
{
	auto f = [argc](auto i)
	{
		return i * i + argc;
	};

	auto s = sizeof(f);

	std::vector<int> v{ 1,1,2,3,5,8,13,21,34 };

	for (auto i : v | std::views::transform(f))
	{
		i = i;
	};



	return 0;
}
