#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<Input2D> const Input2D_metadata =
	{
		{ Input2D::UNKNOWN, "UNKNOWN" },

		{ Input2D::MOUSE_DELTA, "MOUSE_DELTA" },
		{ Input2D::MOUSE_WHEEL, "MOUSE_WHEEL" },

		{ Input2D::GAMEPAD_LEFT_STICK, "LEFT_STICK" },
		{ Input2D::GAMEPAD_RIGHT_STICK, "RIGHT_STICK" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Input2D, &Input2D_metadata, CHAOS_API);

	bool IsKeyboardInput(Input2D in_input)
	{
		return false;
	}

	bool IsMouseInput(Input2D in_input)
	{
		return (in_input >= Input2D::MOUSE_FIRST) && (in_input <= Input2D::MOUSE_LAST);
	}

	bool IsGamepadInput(Input2D in_input)
	{
		return (in_input >= Input2D::GAMEPAD_FIRST) && (in_input <= Input2D::GAMEPAD_LAST);
	}

}; // namespace chaos
