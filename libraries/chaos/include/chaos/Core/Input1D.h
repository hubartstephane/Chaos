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
		/** undefined value */
		UNKNOWN       = -1,
		GAMEPAD_LEFT_AXIS_X   = GLFW_GAMEPAD_AXIS_LEFT_X,
		GAMEPAD_LEFT_AXIS_Y   = GLFW_GAMEPAD_AXIS_LEFT_Y,   // for GLFW stick down = positive values (we would rather use positive values for UP)
		GAMEPAD_RIGHT_AXIS_X  = GLFW_GAMEPAD_AXIS_RIGHT_X,
		GAMEPAD_RIGHT_AXIS_Y  = GLFW_GAMEPAD_AXIS_RIGHT_Y,  // for GLFW stick down = positive values (we would rather use positive values for UP)
		GAMEPAD_LEFT_TRIGGER  = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
		GAMEPAD_RIGHT_TRIGGER = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
	};

	CHAOS_DECLARE_ENUM_METHOD(Input1D, CHAOS_API);

#endif

}; // namespace chaos