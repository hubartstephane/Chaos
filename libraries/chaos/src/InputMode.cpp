#include <chaos/InputMode.h>

namespace chaos
{
	bool InputMode::IsPCMode(int mode)
	{
		return (mode == Keyboard) || (mode == Mouse);
	}

	bool InputMode::IsConsoleMode(int mode)
	{
		return (mode == Gamepad);
	}

	bool InputMode::IsPlatformChanged(int new_mode, int old_mode)
	{
		return (new_mode == Gamepad) ^ (old_mode == Gamepad); // one and only one is gamepad
	}

}; // namespace chaos
