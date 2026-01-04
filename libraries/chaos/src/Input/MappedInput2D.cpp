#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	MappedInput2D const MappedInput2D::default_keyboard_mapping =
	{
		Key::LEFT,
		Key::RIGHT,
		Key::DOWN,
		Key::UP
	};

	MappedInput2D const MappedInput2D::default_dpad_mapping =
	{
		Key::GAMEPAD_DPAD_LEFT,
		Key::GAMEPAD_DPAD_RIGHT,
		Key::GAMEPAD_DPAD_DOWN,
		Key::GAMEPAD_DPAD_UP
	};

	MappedInput2D const MappedInput2D::UNKNOWN =
	{
		Key::UNKNOWN,
		Key::UNKNOWN,
		Key::UNKNOWN,
		Key::UNKNOWN
	};

}; // namespace chaos
