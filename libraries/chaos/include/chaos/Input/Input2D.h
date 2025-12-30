namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Input2D;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * Input2D: enumeration to match a 2D input axis
	 */

	enum class CHAOS_API Input2D : int
	{
		UNKNOWN     = -1,

		MOUSE_FIRST = 1000,
		MOUSE_DELTA = MOUSE_FIRST + 0,
		MOUSE_WHEEL = MOUSE_FIRST + 1,
		MOUSE_LAST  = MOUSE_WHEEL,

		GAMEPAD_FIRST       = 2000,
		GAMEPAD_LEFT_STICK  = GAMEPAD_FIRST + 0,
		GAMEPAD_RIGHT_STICK = GAMEPAD_FIRST + 1,
		GAMEPAD_LAST        = GAMEPAD_RIGHT_STICK
	};

	CHAOS_DECLARE_ENUM_METHOD(Input2D, CHAOS_API);

	CHAOS_API bool IsKeyboardInput(Input2D in_input);

	CHAOS_API bool IsMouseInput(Input2D in_input);

	CHAOS_API bool IsGamepadInput(Input2D in_input);

	CHAOS_API Input2D GetInput2DFromName(char const* in_name);

#endif

}; // namespace chaos