#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<Input1D> const Input1D_metadata =
	{
		{ Input1D::UNKNOWN, "UNKNOWN" },
		{ Input1D::GAMEPAD_LEFT_AXIS_X, "LEFT_AXIS_X" },
		{ Input1D::GAMEPAD_LEFT_AXIS_Y, "LEFT_AXIS_Y" },
		{ Input1D::GAMEPAD_RIGHT_AXIS_X, "RIGHT_AXIS_X" },
		{ Input1D::GAMEPAD_RIGHT_AXIS_Y, "RIGHT_AXIS_Y" },
		{ Input1D::GAMEPAD_LEFT_TRIGGER, "LEFT_TRIGGER" },
		{ Input1D::GAMEPAD_RIGHT_TRIGGER, "RIGHT_TRIGGER" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Input1D, &Input1D_metadata, CHAOS_API);

	bool IsKeyboardInput(Input1D in_input)
	{
		return false;
	}

	bool IsMouseInput(Input1D in_input)
	{
		return (in_input >= Input1D::MOUSE_FIRST) && (in_input <= Input1D::MOUSE_LAST);
	}

	bool IsGamepadInput(Input1D in_input)
	{
		return (in_input >= Input1D::GAMEPAD_FIRST) && (in_input <= Input1D::GAMEPAD_LAST);
	}

}; // namespace chaos
