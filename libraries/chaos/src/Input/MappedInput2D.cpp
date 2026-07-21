#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	MappedInput2D const MappedInput2D::keyboard_arrows =
	{
		Key::Left,
		Key::Right,
		Key::Down,
		Key::Up
	};

	MappedInput2D const MappedInput2D::gamepad_dpad =
	{
		Key::GamepadDpadLeft,
		Key::GamepadDpadRight,
		Key::GamepadDpadDown,
		Key::GamepadDpadUp
	};

	MappedInput2D const MappedInput2D::Unknown =
	{
		Key::Unknown,
		Key::Unknown,
		Key::Unknown,
		Key::Unknown
	};

}; // namespace chaos
