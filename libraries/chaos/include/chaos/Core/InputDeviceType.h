namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class InputDeviceType;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InputDeviceType: Indicates where the input is coming from
	 */

	enum class CHAOS_API InputDeviceType : int
	{
		UNKNOWN  = -1,
		KEYBOARD = 0,
		MOUSE    = 1,
		GAMEPAD  = 2
	};

	CHAOS_DECLARE_ENUM_METHOD(InputDeviceType, CHAOS_API);

#endif

}; // namespace chaos