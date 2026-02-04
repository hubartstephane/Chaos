namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Input1D;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * Input1D: enumeration to match a 1D input axis
	 */

	enum class Input1D : int
	{
		Unknown       = -1,

		MouseFirst = 1000,
		MouseWheelX = MouseFirst + 0,
		MouseWheelY = MouseFirst + 1,
		MouseLast  = MouseWheelY,

		GamepadFirst = 2000,
		GamepadLeftAxisX    = GamepadFirst + GLFW_GAMEPAD_AXIS_LEFT_X,
		GamepadLeftAxisY    = GamepadFirst + GLFW_GAMEPAD_AXIS_LEFT_Y,   // for GLFW stick down = positive values (we would rather use positive values for UP)
		GamepadRightAxisX   = GamepadFirst + GLFW_GAMEPAD_AXIS_RIGHT_X,
		GamepadRightAxisY   = GamepadFirst + GLFW_GAMEPAD_AXIS_RIGHT_Y,  // for GLFW stick down = positive values (we would rather use positive values for UP)
		GamepadLeftTrigger  = GamepadFirst + GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
		GamepadRightTrigger = GamepadFirst + GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
		GamepadLast = GamepadRightTrigger,
	};

	CHAOS_DECLARE_ENUM_METHOD(Input1D, CHAOS_API);

	CHAOS_API bool IsKeyboardInput(Input1D in_input);

	CHAOS_API bool IsMouseInput(Input1D in_input);

	CHAOS_API bool IsGamepadInput(Input1D in_input);

	CHAOS_API Input1D GetInput1DFromName(char const* in_name);

#endif

}; // namespace chaos