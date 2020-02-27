#pragma once

#include <death/GameWindow.h>

#include <chaos/StandardHeaders.h>
#include <chaos/MyGLFWSingleWindowApplication.h>

namespace death
{
	template<typename GAME_TYPE>
	bool RunGame(int argc, char ** argv, char ** env, chaos::MyGLFW::SingleWindowApplicationParams params = chaos::MyGLFW::SingleWindowApplicationParams())
	{
		return chaos::MyGLFW::RunWindowApplication<TypedGameWindow<GAME_TYPE>>(argc, argv, env, params);
	}

}; // namespace death
