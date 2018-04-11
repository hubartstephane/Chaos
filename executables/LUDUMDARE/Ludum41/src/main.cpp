#include <chaos/StandardHeaders.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 

#include "LudumWindow.h"
#include "LudumGame.h"

// ======================================================================================


int _tmain(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<LudumWindow>(argc, argv, env, params);
	return 0;
}


