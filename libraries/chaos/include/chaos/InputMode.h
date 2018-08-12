#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	class InputMode
	{
	public:

		/** the mode for keyboard */
		static int const Keyboard = 0;
		/** the mode for Mouse */
		static int const Mouse    = 1;
		/** the mode for Gamepad */
		static int const Gamepad  = 2;	

		/** returns true whether given mode correspond to mouse or keyboard */
		static bool IsPCMode(int mode);
		/** returns true whether given mode correspond to gamepad */
		static bool IsConsoleMode(int mode);
		/** returns true whether we change platform (PC to console or inverse) */
		static bool IsPlatformChanged(int new_mode, int old_mode);
	};
}; // namespace chaos