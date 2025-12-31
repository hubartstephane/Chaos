#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	MappedInput1D const MappedInput1D::default_keyboard_mapping =
	{
		Key::LEFT,
		Key::RIGHT
	};

	MappedInput1D const MappedInput1D::default_dpad_mapping =
	{
		Key::GAMEPAD_DPAD_LEFT,
		Key::GAMEPAD_DPAD_RIGHT
	};

	MappedInput2D const MappedInput2D::default_keyboard_mapping =
	{
		Key::LEFT,
		Key::RIGHT,
		Key::UP,
		Key::DOWN
	};

	MappedInput2D const MappedInput2D::default_dpad_mapping =
	{
		Key::GAMEPAD_DPAD_LEFT,
		Key::GAMEPAD_DPAD_RIGHT,
		Key::GAMEPAD_DPAD_UP,
		Key::GAMEPAD_DPAD_DOWN
	};

}; // namespace chaos
