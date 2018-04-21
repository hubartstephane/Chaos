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

}; // namespace chaos
