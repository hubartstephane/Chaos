#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 
#include <chaos/STLTools.h> 

#include "Ludum46Game.h"






int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	using namespace std;

	vector<pair<int, int>> v
	

	 {

		{13, 17}
#if 0
		{22, 27},
		{49, 60},
		{70, 80},
		{90, 100},
		{110, 120}
#endif
	};


	for (int gid : {12, 13, 15, 25, 50, 75, 90, 120, 19, 12, 121, 82})
	{



		auto it = chaos::STLTools::FindSortedVector(v, gid, [](pair<int, int> const& p, int gid)
		{
			if (gid > p.second)
				return 1;
			else if (gid < p.first)
				return -1;
			else
				return 0;
		});


		if (it == v.end())
		{
			it = it;

		}
		else
		{
			auto r = *it;

			it = it;
		}
	}














	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}
