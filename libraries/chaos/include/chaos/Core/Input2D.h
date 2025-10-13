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
		GAMEPAD_LEFT_STICK  = 0,
		GAMEPAD_RIGHT_STICK = 1
	};

	CHAOS_DECLARE_ENUM_METHOD(Input2D, CHAOS_API);

#endif

}; // namespace chaos