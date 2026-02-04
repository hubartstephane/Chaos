namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class InputDeviceType;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InputDeviceType: Indicates where the input is coming from
	 */

	enum class InputDeviceType : int
	{
		Unknown  = -1,
		Keyboard = 0,
		Mouse    = 1,
		Gamepad  = 2
	};

	CHAOS_DECLARE_ENUM_METHOD(InputDeviceType, CHAOS_API);

#endif

}; // namespace chaos