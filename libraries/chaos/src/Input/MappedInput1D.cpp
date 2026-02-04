#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	MappedInput1D const MappedInput1D::default_keyboard_mapping =
	{
		Key::Left,
		Key::Right
	};

	MappedInput1D const MappedInput1D::default_dpad_mapping =
	{
		Key::GamepadDpadLeft,
		Key::GamepadDpadRight
	};

	MappedInput1D const MappedInput1D::UNKNOWN =
	{
		Key::Unknown,
		Key::Unknown
	};

}; // namespace chaos
