namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Input1D;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * Input1D: enumeration to match a 1D input axis
	 */

	enum class CHAOS_API Input1D : int
	{
		UNKNOWN       = -1,

		MOUSE_FIRST = 1000,
		MOUSE_WHEEL_X = MOUSE_FIRST + 0,
		MOUSE_WHEEL_Y = MOUSE_FIRST + 1,
		MOUSE_LAST  = MOUSE_WHEEL_Y,

		GAMEPAD_FIRST = 2000,
		GAMEPAD_LEFT_AXIS_X   = GAMEPAD_FIRST + GLFW_GAMEPAD_AXIS_LEFT_X,
		GAMEPAD_LEFT_AXIS_Y   = GAMEPAD_FIRST + GLFW_GAMEPAD_AXIS_LEFT_Y,   // for GLFW stick down = positive values (we would rather use positive values for UP)
		GAMEPAD_RIGHT_AXIS_X  = GAMEPAD_FIRST + GLFW_GAMEPAD_AXIS_RIGHT_X,
		GAMEPAD_RIGHT_AXIS_Y  = GAMEPAD_FIRST + GLFW_GAMEPAD_AXIS_RIGHT_Y,  // for GLFW stick down = positive values (we would rather use positive values for UP)
		GAMEPAD_LEFT_TRIGGER  = GAMEPAD_FIRST + GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
		GAMEPAD_RIGHT_TRIGGER = GAMEPAD_FIRST + GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
		GAMEPAD_LAST = GAMEPAD_RIGHT_TRIGGER,
	};

	CHAOS_DECLARE_ENUM_METHOD(Input1D, CHAOS_API);

	CHAOS_API bool IsKeyboardInput(Input1D in_input);

	CHAOS_API bool IsMouseInput(Input1D in_input);

	CHAOS_API bool IsGamepadInput(Input1D in_input);

	CHAOS_API Input1D GetInput1DFromName(char const* in_name);

#endif

}; // namespace chaos