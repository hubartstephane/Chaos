#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	MappedInput2D const MappedInput2D::default_keyboard_mapping =
	{
		Key::Left,
		Key::Right,
		Key::Down,
		Key::Up
	};

	MappedInput2D const MappedInput2D::default_dpad_mapping =
	{
		Key::GamepadDpadLeft,
		Key::GamepadDpadRight,
		Key::GamepadDpadDown,
		Key::GamepadDpadUp
	};

	MappedInput2D const MappedInput2D::UNKNOWN =
	{
		Key::Unknown,
		Key::Unknown,
		Key::Unknown,
		Key::Unknown
	};

}; // namespace chaos
