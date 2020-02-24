#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	enum class InputMode : int
	{
		/** the mode for keyboard */
		KEYBOARD = 0,
		/** the mode for Mouse */
		MOUSE    = 1,
		/** the mode for Gamepad */
		GAMEPAD  = 2	
	};

	/** returns true whether given mode correspond to mouse or keyboard */
	bool IsPCMode(InputMode mode);
	/** returns true whether given mode correspond to gamepad */
	bool IsConsoleMode(InputMode mode);
	/** returns true whether we change platform (PC to console or inverse) */
	bool IsPlatformChanged(InputMode new_mode, InputMode old_mode);

}; // namespace chaos