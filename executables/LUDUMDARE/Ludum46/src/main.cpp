#include <chaos/Chaos.h>

#include "Ludum46Game.h"

#include <boost/type_traits/add_const.hpp>


auto ggg(std::vector<int>::iterator it)
{
	static std::vector<int> v = { 0, 2, 4, 6 };
	return std::make_pair(v.begin(), v.end());
}

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	std::vector<int> v = { 1, 2, 3 };

	auto it = chaos::NestedIterator< std::vector<int>::iterator, std::vector<int>::iterator, ggg>(v.begin(), v.end());

	int count = 0;

	while (it)
	{
		auto value = *it;
		value = value;
		++it;
		++count;
	}
	count = count;

	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}
