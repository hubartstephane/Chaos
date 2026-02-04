#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	static EnumMetaData<Input1D> const Input1D_metadata =
	{
		{ Input1D::Unknown, "Unknown" },

		{ Input1D::MouseWheelX, "MouseWheelX" },
		{ Input1D::MouseWheelY, "MouseWheelY" },

		{ Input1D::GamepadLeftAxisX, "LeftAxisX" },
		{ Input1D::GamepadLeftAxisY, "LeftAxisY" },
		{ Input1D::GamepadRightAxisX, "RightAxisX" },
		{ Input1D::GamepadRightAxisY, "RightAxisY" },
		{ Input1D::GamepadLeftTrigger, "LeftTrigger" },
		{ Input1D::GamepadRightTrigger, "RightTrigger" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(Input1D, &Input1D_metadata, CHAOS_API);

	bool IsKeyboardInput(Input1D in_input)
	{
		return false;
	}

	bool IsMouseInput(Input1D in_input)
	{
		return (in_input >= Input1D::MouseFirst) && (in_input <= Input1D::MouseLast);
	}

	bool IsGamepadInput(Input1D in_input)
	{
		return (in_input >= Input1D::GamepadFirst) && (in_input <= Input1D::GamepadLast);
	}

	Input1D GetInput1DFromName(char const* in_name)
	{
		Input1D result = Input1D::Unknown;
		if (StringToEnum(in_name, result))
			return result;
		return Input1D::Unknown;
	}

}; // namespace chaos
