#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	MappedInput1D const MappedInput1D::keyboard_horizontal_arrows =
	{
		Key::Left,
		Key::Right
	};

	MappedInput1D const MappedInput1D::keyboard_vertical_arrows =
	{
		Key::Down,
		Key::Up
	};

	MappedInput1D const MappedInput1D::gamepad_horizontal_dpad =
	{
		Key::GamepadDpadLeft,
		Key::GamepadDpadRight
	};

	MappedInput1D const MappedInput1D::gamepad_vertical_dpad =
	{
		Key::GamepadDpadDown,
		Key::GamepadDpadUp
	};

	MappedInput1D const MappedInput1D::Unknown =
	{
		Key::Unknown,
		Key::Unknown
	};

}; // namespace chaos
