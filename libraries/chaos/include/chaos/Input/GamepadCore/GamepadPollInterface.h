namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GamepadPollInterface: an interface for objects that want to poll gamepad states
	*/

	class GamepadPollInterface
	{
		CHAOS_GAMEPAD_ALL_FRIENDS;

	public:

		/** called whenever a physical state is to be polled */
		virtual bool DoPollGamepad(PhysicalGamepad * physical_gamepad);
	};

#endif

}; // namespace chaos