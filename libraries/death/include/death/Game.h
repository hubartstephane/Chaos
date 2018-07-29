#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/MyGLFWGamepadManager.h>


namespace death
{
	class Game : public chaos::ReferencedObject
	{
		friend class GamepadManager;

	protected:

		/** called whenever a gamepad input is comming */
		virtual bool OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);

	protected:

		/** the gamepad data */
		chaos::MyGLFW::GamepadData gamepad_data;
	};

}; // namespace death
