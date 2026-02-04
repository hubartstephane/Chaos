#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<Input2D> const Input2D_metadata =
	{
		{ Input2D::Unknown, "Unknown" },

		{ Input2D::MouseDelta, "MouseDelta" },
		{ Input2D::MouseWheel, "MouseWheel" },

		{ Input2D::GamepadLeftStick, "LeftStick" },
		{ Input2D::GamepadRightStick, "RightStick" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Input2D, &Input2D_metadata, CHAOS_API);

	bool IsKeyboardInput(Input2D in_input)
	{
		return false;
	}

	bool IsMouseInput(Input2D in_input)
	{
		return (in_input >= Input2D::MouseFirst) && (in_input <= Input2D::MouseLast);
	}

	bool IsGamepadInput(Input2D in_input)
	{
		return (in_input >= Input2D::GamepadFirst) && (in_input <= Input2D::GamepadLast);
	}

	Input2D GetInput2DFromName(char const* in_name)
	{
		Input2D result = Input2D::Unknown;
		if (StringToEnum(in_name, result))
			return result;
		return Input2D::Unknown;
	}

}; // namespace chaos
