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

}; // namespace chaos
