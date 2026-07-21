#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	MappedInput1D const MappedInput1D::keyboard_arrows =
	{
		Key::Left,
		Key::Right
	};

	MappedInput1D const MappedInput1D::gamepad_dpad =
	{
		Key::GamepadDpadLeft,
		Key::GamepadDpadRight
	};

	MappedInput1D const MappedInput1D::Unknown =
	{
		Key::Unknown,
		Key::Unknown
	};

}; // namespace chaos
