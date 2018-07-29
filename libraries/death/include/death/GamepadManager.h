#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/MyGLFWGamepadManager.h>

#include <death/Game.h>

namespace death
{
	class GamepadManager : public chaos::MyGLFW::GamepadManager
	{
	public:

		/** the constructor */
		GamepadManager(Game * in_game) : game(in_game)
		{
			assert(nullptr);
		}

	protected:

		/** the gamepad manager */
		virtual bool DoPoolGamepad(chaos::MyGLFW::PhysicalGamepad * physical_gamepad) override;

	protected:

		/** pointer on the game */
		Game * game = nullptr;
	};

}; // namespace death
