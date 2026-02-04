namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Input2D;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * Input2D: enumeration to match a 2D input axis
	 */

	enum class Input2D : int
	{
		Unknown     = -1,

		MouseFirst = 1000,
		MouseDelta = MouseFirst + 0,
		MouseWheel = MouseFirst + 1,
		MouseLast  = MouseWheel,

		GamepadFirst      = 2000,
		GamepadLeftStick  = GamepadFirst + 0,
		GamepadRightStick = GamepadFirst + 1,
		GamepadLast       = GamepadRightStick
	};

	CHAOS_DECLARE_ENUM_METHOD(Input2D, CHAOS_API);

	CHAOS_API bool IsKeyboardInput(Input2D in_input);

	CHAOS_API bool IsMouseInput(Input2D in_input);

	CHAOS_API bool IsGamepadInput(Input2D in_input);

	CHAOS_API Input2D GetInput2DFromName(char const* in_name);

#endif

}; // namespace chaos