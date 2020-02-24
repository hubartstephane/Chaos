#include <chaos/InputMode.h>

namespace chaos
{
	bool IsPCMode(InputMode mode)
	{
		return (mode == InputMode::KEYBOARD) || (mode == InputMode::MOUSE);
	}

	bool IsConsoleMode(InputMode mode)
	{
		return (mode == InputMode::GAMEPAD);
	}

	bool IsPlatformChanged(InputMode new_mode, InputMode old_mode)
	{
		return IsConsoleMode(new_mode) != IsConsoleMode(old_mode); // one and only one is gamepad
	}

}; // namespace chaos
