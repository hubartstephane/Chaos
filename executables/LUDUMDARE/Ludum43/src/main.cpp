#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 




#include "Ludum43Game.h"

#include <chaos/Buffer.h> 
#include <chaos/Application.h> 
#include <chaos/FilePath.h> 
#include <chaos/FileTools.h> 

int _tmain(int argc, char ** argv, char ** env)
{
	bool open_success = false;
	chaos::Buffer<char> buffer = chaos::FileTools::LoadFile("D:\\Personnel\\Programmation\\build\\vs2015\\executables\\LUDUMDARE\\Ludum43\\DEBUG\\x32\\resources\\config.json", true, &open_success);

	open_success = open_success;
	return 0;

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


