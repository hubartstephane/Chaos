#include <chaos/StandardHeaders.h> 

#include <death/Death.h> 

#include "Ludum42Game.h"

#include <chaos/Renderable.h>

using namespace chaos;

int _tmain(int argc, char ** argv, char ** env)
{
	RenderableLayer * layer = new RenderableLayer;

	Renderable * r1 = nullptr;
	Renderable * r2 = nullptr;
	Renderable * r3 = nullptr;
	Renderable * r4 = nullptr;
	Renderable * r5 = nullptr;

	layer->AddChildRenderable(r1 = new Renderable, 3);
	layer->AddChildRenderable(r2 = new Renderable, 9);
	layer->AddChildRenderable(r3 = new Renderable, 4);
	layer->AddChildRenderable(r4 = new Renderable, 10);
	layer->AddChildRenderable(r5 = new Renderable, 9);
	
	layer->RemoveChildRenderable(r3);



	return 0;

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


