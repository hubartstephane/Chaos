#include <chaos/StandardHeaders.h> 


#include <chaos/WinTools.h>  // shuxxx


#include <death/Death.h> 




#include "Ludum43Game.h"

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{

	bool azerty = chaos::WinTools::IsAzertyKeyboard();
	azerty = azerty;


	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


