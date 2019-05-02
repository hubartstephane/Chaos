#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 
#include "Ludum43Game.h"

#include <chaos/EmptyClass.h>
#include <chaos/MetaProgramming.h>

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{

	{
		A * a = new A;
		B * b = new B;
		C * c = new C;

		A * a1 = chaos::cast(a);
		B * b1 = chaos::cast(a);
		C * c1 = chaos::cast(a);

		A * a2 = chaos::cast(b);
		B * b2 = chaos::cast(b);
		C * c2 = chaos::cast(b);

		A * a3 = chaos::cast(c);
		B * b3 = chaos::cast(c);
		C * c3 = chaos::cast(c);

		argc = argc;
	}
	{
		A const * a = new A;
		B const * b = new B;
		C const * c = nullptr; // new C;

		A const * a1 = chaos::cast(a);
		B const * b1 = chaos::cast(a);
		C const * c1 = chaos::cast(a);

		A const * a2 = chaos::cast(b);
		B const * b2 = chaos::cast(b);
		C const * c2 = chaos::cast(b);

		A const * a3 = chaos::cast(c);
		B const * b3 = chaos::cast(c);
		C const * c3 = chaos::cast(c);

		argc = argc;
	}


	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


